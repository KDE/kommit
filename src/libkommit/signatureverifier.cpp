/*
SPDX-FileCopyrightText: 2026 Li Bohai <lbhlbhlbh2002@icloud.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signatureverifier.h"

#include "libkommit_debug.h"

#include <QByteArray>
#include <QString>

#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/oid.h>
#include <git2/repository.h>

#include <array>
#include <cstring>

using namespace Git;

namespace
{

enum class SshKeyType {
    Unknown,
    Rsa,
    Ecdsa256,
    Ecdsa384,
    Ecdsa521,
    Ed25519
};

struct SshStringReader {
    const unsigned char *data;
    size_t len;
    size_t pos{0};

    bool readU32(uint32_t &out)
    {
        if (pos + 4 > len)
            return false;
        out = (static_cast<uint32_t>(data[pos]) << 24) | (static_cast<uint32_t>(data[pos + 1]) << 16) | (static_cast<uint32_t>(data[pos + 2]) << 8)
            | static_cast<uint32_t>(data[pos + 3]);
        pos += 4;
        return true;
    }

    bool readString(QByteArray &out)
    {
        uint32_t slen;
        if (!readU32(slen))
            return false;
        if (pos + slen > len)
            return false;
        out = QByteArray(reinterpret_cast<const char *>(data + pos), static_cast<int>(slen));
        pos += slen;
        return true;
    }

    bool readMpint(QByteArray &out)
    {
        QByteArray raw;
        if (!readString(raw))
            return false;
        if (raw.isEmpty()) {
            out = QByteArray(1, '\0');
            return true;
        }
        if (raw.at(0) == '\0' && raw.size() > 1 && (static_cast<unsigned char>(raw.at(1)) & 0x80)) {
            out = raw.mid(1);
        } else {
            out = raw;
        }
        return true;
    }
};

SshKeyType parseSshKeyType(const QByteArray &keyTypeStr)
{
    if (keyTypeStr == "ssh-rsa")
        return SshKeyType::Rsa;
    if (keyTypeStr == "ecdsa-sha2-nistp256")
        return SshKeyType::Ecdsa256;
    if (keyTypeStr == "ecdsa-sha2-nistp384")
        return SshKeyType::Ecdsa384;
    if (keyTypeStr == "ecdsa-sha2-nistp521")
        return SshKeyType::Ecdsa521;
    if (keyTypeStr == "ssh-ed25519")
        return SshKeyType::Ed25519;
    return SshKeyType::Unknown;
}

} // anonymous namespace

#ifdef LIBKOMMIT_USE_OPENSSL
#include <openssl/bn.h>
#include <openssl/core_names.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <openssl/param_build.h>
#include <openssl/params.h>
#include <openssl/rsa.h>

static int sshCurveToOpenSslNid(SshKeyType type)
{
    switch (type) {
    case SshKeyType::Ecdsa256:
        return NID_X9_62_prime256v1;
    case SshKeyType::Ecdsa384:
        return NID_secp384r1;
    case SshKeyType::Ecdsa521:
        return NID_secp521r1;
    default:
        return NID_undef;
    }
}

static const EVP_MD *hashNameToEvpMd(const QByteArray &name)
{
    if (name == "sha256")
        return EVP_sha256();
    if (name == "sha512")
        return EVP_sha512();
    return nullptr;
}

static bool verifySshSignatureOpenSsl(const QByteArray &signedData, const QByteArray &signatureBlob)
{
    SshStringReader reader{reinterpret_cast<const unsigned char *>(signatureBlob.constData()), static_cast<size_t>(signatureBlob.size()), 0};

    if (reader.len < 6 || memcmp(reader.data, "SSHSIG", 6) != 0)
        return false;
    reader.pos += 6;

    uint32_t version;
    if (!reader.readU32(version))
        return false;
    if (version != 1)
        return false;

    QByteArray publicKeyBlob;
    if (!reader.readString(publicKeyBlob))
        return false;

    QByteArray namespaceStr;
    if (!reader.readString(namespaceStr))
        return false;

    QByteArray reserved;
    if (!reader.readString(reserved))
        return false;

    QByteArray hashAlgorithm;
    if (!reader.readString(hashAlgorithm))
        return false;

    QByteArray signature;
    if (!reader.readString(signature))
        return false;

    SshStringReader pkReader{reinterpret_cast<const unsigned char *>(publicKeyBlob.constData()), static_cast<size_t>(publicKeyBlob.size()), 0};

    QByteArray keyTypeStr;
    if (!pkReader.readString(keyTypeStr))
        return false;
    SshKeyType keyType = parseSshKeyType(keyTypeStr);
    if (keyType == SshKeyType::Unknown)
        return false;

    EVP_PKEY *pkey = nullptr;

    if (keyType == SshKeyType::Rsa) {
        QByteArray eBytes, nBytes;
        if (!pkReader.readMpint(eBytes))
            return false;
        if (!pkReader.readMpint(nBytes))
            return false;

        BIGNUM *e = BN_bin2bn(reinterpret_cast<const unsigned char *>(eBytes.constData()), static_cast<int>(eBytes.size()), nullptr);
        BIGNUM *n = BN_bin2bn(reinterpret_cast<const unsigned char *>(nBytes.constData()), static_cast<int>(nBytes.size()), nullptr);
        if (!e || !n) {
            BN_free(e);
            BN_free(n);
            return false;
        }

        OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();
        OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_N, n);
        OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_RSA_E, e);
        OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(bld);
        OSSL_PARAM_BLD_free(bld);

        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_from_name(nullptr, "RSA", nullptr);
        if (!pctx || EVP_PKEY_fromdata_init(pctx) <= 0 || EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_PUBLIC_KEY, params) <= 0) {
            EVP_PKEY_CTX_free(pctx);
            OSSL_PARAM_free(params);
            BN_free(e);
            BN_free(n);
            return false;
        }
        EVP_PKEY_CTX_free(pctx);
        OSSL_PARAM_free(params);
        BN_free(e);
        BN_free(n);

    } else if (keyType == SshKeyType::Ecdsa256 || keyType == SshKeyType::Ecdsa384 || keyType == SshKeyType::Ecdsa521) {
        QByteArray curveName;
        if (!pkReader.readString(curveName))
            return false;

        QByteArray qBytes;
        if (!pkReader.readString(qBytes))
            return false;

        int nid = sshCurveToOpenSslNid(keyType);
        if (nid == NID_undef)
            return false;

        const char *sn = OBJ_nid2sn(nid);
        if (!sn)
            return false;

        OSSL_PARAM_BLD *bld = OSSL_PARAM_BLD_new();
        OSSL_PARAM_BLD_push_utf8_string(bld, OSSL_PKEY_PARAM_GROUP_NAME, sn, 0);
        OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, reinterpret_cast<const unsigned char *>(qBytes.constData()), qBytes.size());
        OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(bld);
        OSSL_PARAM_BLD_free(bld);

        EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        if (!pctx || EVP_PKEY_fromdata_init(pctx) <= 0 || EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_PUBLIC_KEY, params) <= 0) {
            EVP_PKEY_CTX_free(pctx);
            OSSL_PARAM_free(params);
            return false;
        }
        EVP_PKEY_CTX_free(pctx);
        OSSL_PARAM_free(params);

    } else if (keyType == SshKeyType::Ed25519) {
        QByteArray pkBytes;
        if (!pkReader.readString(pkBytes))
            return false;

        pkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_ED25519, nullptr, reinterpret_cast<const unsigned char *>(pkBytes.constData()), pkBytes.size());
        if (!pkey)
            return false;
    }

    if (!pkey)
        return false;

    SshStringReader sigReader{reinterpret_cast<const unsigned char *>(signature.constData()), static_cast<size_t>(signature.size()), 0};

    QByteArray sigKeyType;
    if (!sigReader.readString(sigKeyType)) {
        EVP_PKEY_free(pkey);
        return false;
    }

    QByteArray sigBytes;
    if (keyType == SshKeyType::Rsa) {
        QByteArray rsaSig;
        if (!sigReader.readMpint(rsaSig)) {
            EVP_PKEY_free(pkey);
            return false;
        }
        sigBytes = rsaSig;
    } else if (keyType == SshKeyType::Ecdsa256 || keyType == SshKeyType::Ecdsa384 || keyType == SshKeyType::Ecdsa521) {
        QByteArray rBytes, sBytes;
        if (!sigReader.readMpint(rBytes)) {
            EVP_PKEY_free(pkey);
            return false;
        }
        if (!sigReader.readMpint(sBytes)) {
            EVP_PKEY_free(pkey);
            return false;
        }

        BIGNUM *r = BN_bin2bn(reinterpret_cast<const unsigned char *>(rBytes.constData()), static_cast<int>(rBytes.size()), nullptr);
        BIGNUM *s = BN_bin2bn(reinterpret_cast<const unsigned char *>(sBytes.constData()), static_cast<int>(sBytes.size()), nullptr);
        if (!r || !s) {
            if (r)
                BN_free(r);
            if (s)
                BN_free(s);
            EVP_PKEY_free(pkey);
            return false;
        }

        ECDSA_SIG *ecdsaSig = ECDSA_SIG_new();
        ECDSA_SIG_set0(ecdsaSig, r, s);

        int derLen = i2d_ECDSA_SIG(ecdsaSig, nullptr);
        sigBytes.resize(derLen);
        unsigned char *derPtr = reinterpret_cast<unsigned char *>(sigBytes.data());
        i2d_ECDSA_SIG(ecdsaSig, &derPtr);
        ECDSA_SIG_free(ecdsaSig);
    } else if (keyType == SshKeyType::Ed25519) {
        if (!sigReader.readString(sigBytes)) {
            EVP_PKEY_free(pkey);
            return false;
        }
    }

    const EVP_MD *hashMd = hashNameToEvpMd(hashAlgorithm);
    if (!hashMd) {
        EVP_PKEY_free(pkey);
        return false;
    }

    EVP_MD_CTX *hctx = EVP_MD_CTX_new();
    if (!hctx) {
        EVP_PKEY_free(pkey);
        return false;
    }
    EVP_DigestInit(hctx, hashMd);
    EVP_DigestUpdate(hctx, signedData.constData(), signedData.size());
    unsigned char messageHash[EVP_MAX_MD_SIZE];
    unsigned int messageHashLen = 0;
    EVP_DigestFinal(hctx, messageHash, &messageHashLen);
    EVP_MD_CTX_free(hctx);

    QByteArray verifyPayload;
    verifyPayload.append("SSHSIG", 6);
    auto appendSshString = [&verifyPayload](const QByteArray &s) {
        uint32_t len = static_cast<uint32_t>(s.size());
        verifyPayload.append(static_cast<char>(len >> 24));
        verifyPayload.append(static_cast<char>(len >> 16));
        verifyPayload.append(static_cast<char>(len >> 8));
        verifyPayload.append(static_cast<char>(len));
        verifyPayload.append(s);
    };
    appendSshString(namespaceStr);
    appendSshString(reserved);
    appendSshString(hashAlgorithm);
    appendSshString(QByteArray(reinterpret_cast<const char *>(messageHash), static_cast<int>(messageHashLen)));

    const EVP_MD *verifyMd = (keyType == SshKeyType::Ed25519) ? nullptr : hashMd;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        EVP_PKEY_free(pkey);
        return false;
    }

    bool ok = false;
    if (EVP_DigestVerifyInit(mdctx, nullptr, verifyMd, nullptr, pkey) == 1) {
        if (EVP_DigestVerify(mdctx,
                             reinterpret_cast<const unsigned char *>(sigBytes.constData()),
                             sigBytes.size(),
                             reinterpret_cast<const unsigned char *>(verifyPayload.constData()),
                             verifyPayload.size())
            == 1) {
            ok = true;
        }
    }

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);

    return ok;
}
#else
static bool verifySshSignatureOpenSsl(const QByteArray &, const QByteArray &)
{
    qDebug() << "OpenSSL support is not available, cannot verify SSH signature";
    return false;
}
#endif // LIBKOMMIT_USE_OPENSSL

#ifdef LIBKOMMIT_USE_GPGME
#include <gpgme.h>

static CommitSignatureInfo verifyGpgWithGpgme(const QByteArray &signature, const QByteArray &signedData)
{
    CommitSignatureInfo result;
    result.setType(CommitSignatureInfo::GpgSignature);

    gpgme_check_version(nullptr);

    gpgme_ctx_t ctx;
    gpgme_error_t err = gpgme_new(&ctx);
    if (err) {
        result.setStatus(CommitSignatureInfo::Error);
        return result;
    }

    gpgme_set_protocol(ctx, GPGME_PROTOCOL_OpenPGP);

    gpgme_data_t sigData, textData;
    err = gpgme_data_new_from_mem(&sigData, signature.constData(), signature.size(), 0);
    if (err) {
        gpgme_release(ctx);
        result.setStatus(CommitSignatureInfo::Error);
        return result;
    }

    err = gpgme_data_new_from_mem(&textData, signedData.constData(), signedData.size(), 0);
    if (err) {
        gpgme_data_release(sigData);
        gpgme_release(ctx);
        result.setStatus(CommitSignatureInfo::Error);
        return result;
    }

    err = gpgme_op_verify(ctx, sigData, textData, nullptr);
    gpgme_data_release(sigData);
    gpgme_data_release(textData);

    if (err) {
        gpgme_release(ctx);
        result.setStatus(CommitSignatureInfo::Error);
        return result;
    }

    gpgme_verify_result_t verifyResult = gpgme_op_verify_result(ctx);
    if (!verifyResult || !verifyResult->signatures) {
        gpgme_release(ctx);
        result.setStatus(CommitSignatureInfo::Bad);
        return result;
    }

    auto *sig = verifyResult->signatures;

    if (sig->fpr)
        result.setFingerprint(QString::fromUtf8(sig->fpr));

    gpgme_key_t key;
    err = gpgme_get_key(ctx, sig->fpr, &key, 0);
    if (!err && key) {
        if (key->uids) {
            result.setSigner(QString::fromUtf8(key->uids->name));
            result.setEmail(QString::fromUtf8(key->uids->email));
        }
        if (key->subkeys && key->subkeys->keyid)
            result.setKeyId(QString::fromUtf8(key->subkeys->keyid));
        gpgme_key_unref(key);
    }

    auto summary = sig->summary;

    if (summary & GPGME_SIGSUM_VALID || summary & GPGME_SIGSUM_GREEN) {
        result.setStatus(CommitSignatureInfo::Good);
    } else if (summary & GPGME_SIGSUM_RED) {
        result.setStatus(CommitSignatureInfo::Bad);
    } else if (summary & GPGME_SIGSUM_KEY_REVOKED) {
        result.setStatus(CommitSignatureInfo::RevokedKey);
    } else if (summary & GPGME_SIGSUM_KEY_EXPIRED) {
        result.setStatus(CommitSignatureInfo::ExpiredKey);
    } else if (summary & GPGME_SIGSUM_SIG_EXPIRED) {
        result.setStatus(CommitSignatureInfo::ExpiredSig);
    } else if (summary & GPGME_SIGSUM_KEY_MISSING) {
        result.setStatus(CommitSignatureInfo::MissingKey);
    } else if (summary & GPGME_SIGSUM_SYS_ERROR) {
        result.setStatus(CommitSignatureInfo::Error);
    } else {
        result.setStatus(CommitSignatureInfo::Unknown);
    }

    result.setTrustLevel(QString());
    if (summary & GPGME_SIGSUM_VALID)
        result.setTrustLevel(QStringLiteral("full"));
    else if (summary & GPGME_SIGSUM_GREEN)
        result.setTrustLevel(QStringLiteral("marginal"));

    gpgme_release(ctx);
    return result;
}
#else
static CommitSignatureInfo verifyGpgWithGpgme(const QByteArray &, const QByteArray &)
{
    qDebug() << "GPGME support is not available, cannot verify GPG signature";
    CommitSignatureInfo result;
    result.setType(CommitSignatureInfo::GpgSignature);
    result.setStatus(CommitSignatureInfo::Error);
    return result;
}
#endif // LIBKOMMIT_USE_GPGME

namespace Git
{

SignatureVerifier::SignatureVerifier()
{
#ifdef LIBKOMMIT_USE_GPGME
    mGpgmeAvailable = true;
#else
    mGpgmeAvailable = false;
#endif

#ifdef LIBKOMMIT_USE_OPENSSL
    mOpensslAvailable = true;
#else
    mOpensslAvailable = false;
#endif
}

SignatureVerifier::~SignatureVerifier() = default;

CommitSignatureInfo SignatureVerifier::verify(git_repository *repo, const git_oid *commitId)
{
    git_buf signatureBuf = GIT_BUF_INIT;
    git_buf signedDataBuf = GIT_BUF_INIT;

    int rc = git_commit_extract_signature(&signatureBuf, &signedDataBuf, repo, const_cast<git_oid *>(commitId), nullptr);
    if (rc != 0) {
        if (rc == GIT_ENOTFOUND) {
            CommitSignatureInfo info;
            info.setStatus(CommitSignatureInfo::None);
            return info;
        }
        CommitSignatureInfo info;
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    QByteArray signature(signatureBuf.ptr, static_cast<int>(signatureBuf.size));
    QByteArray signedData(signedDataBuf.ptr, static_cast<int>(signedDataBuf.size));

    git_buf_dispose(&signatureBuf);
    git_buf_dispose(&signedDataBuf);

    if (signature.isEmpty()) {
        CommitSignatureInfo info;
        info.setStatus(CommitSignatureInfo::None);
        return info;
    }

    if (signature.contains("-----BEGIN SSH SIGNATURE-----")) {
        return verifySsh(signature, signedData);
    }

    return verifyGpg(signature, signedData);
}

CommitSignatureInfo SignatureVerifier::verifyGpg(const QByteArray &signature, const QByteArray &signedData)
{
    if (!mGpgmeAvailable) {
        CommitSignatureInfo info;
        info.setType(CommitSignatureInfo::GpgSignature);
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }
    return verifyGpgWithGpgme(signature, signedData);
}

CommitSignatureInfo SignatureVerifier::verifySsh(const QByteArray &signature, const QByteArray &signedData)
{
    CommitSignatureInfo info;
    info.setType(CommitSignatureInfo::SshSignature);

    if (!mOpensslAvailable) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    int beginIdx = signature.indexOf("-----BEGIN SSH SIGNATURE-----");
    int endIdx = signature.indexOf("-----END SSH SIGNATURE-----");
    if (beginIdx < 0 || endIdx < 0 || endIdx <= beginIdx) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    beginIdx += static_cast<int>(strlen("-----BEGIN SSH SIGNATURE-----\n"));
    QByteArray base64Data = signature.mid(beginIdx, endIdx - beginIdx).trimmed();

    QByteArray decoded = QByteArray::fromBase64(base64Data);
    if (decoded.isEmpty()) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    SshStringReader blobReader{reinterpret_cast<const unsigned char *>(decoded.constData()), static_cast<size_t>(decoded.size()), 0};

    if (blobReader.len < 6 || memcmp(blobReader.data, "SSHSIG", 6) != 0) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }
    blobReader.pos += 6;

    uint32_t version;
    if (!blobReader.readU32(version) || version != 1) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    QByteArray publicKeyBlob;
    if (!blobReader.readString(publicKeyBlob)) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    SshStringReader pkReader{reinterpret_cast<const unsigned char *>(publicKeyBlob.constData()), static_cast<size_t>(publicKeyBlob.size()), 0};

    QByteArray keyTypeStr;
    if (!pkReader.readString(keyTypeStr)) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }
    SshKeyType keyType = parseSshKeyType(keyTypeStr);
    if (keyType == SshKeyType::Unknown) {
        info.setStatus(CommitSignatureInfo::Error);
        return info;
    }

    info.setSigner(QString::fromUtf8(keyTypeStr));

#ifdef LIBKOMMIT_USE_OPENSSL
    unsigned char fingerprint[EVP_MAX_MD_SIZE];
    unsigned int fpLen = 0;
    if (EVP_Digest(reinterpret_cast<const unsigned char *>(publicKeyBlob.constData()), publicKeyBlob.size(), fingerprint, &fpLen, EVP_sha256(), nullptr)) {
        info.setKeyId(QStringLiteral("SHA256:%1")
                          .arg(QString::fromUtf8(
                              QByteArray(reinterpret_cast<const char *>(fingerprint), static_cast<int>(fpLen)).toBase64(QByteArray::OmitTrailingEquals))));
    }
#endif

    if (verifySshSignatureOpenSsl(signedData, decoded)) {
        info.setStatus(CommitSignatureInfo::Good);
    } else {
        info.setStatus(CommitSignatureInfo::Bad);
    }

    return info;
}

}
