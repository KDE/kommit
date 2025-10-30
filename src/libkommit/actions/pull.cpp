#include "pull.h"

#include "fetch.h"
#include "remote.h"

namespace Git
{

class PullPrivate
{
    Fetch *fetch;
    Remote *remote;
};

Pull::Pull(QObject *parent)
    : AbstractAction{parent}
{
}

int Pull::exec()
{
    // git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
    // git_remote_fetch(d->remote->data(), NULL, &fetch_opts, "fetch");
    return 0;

}

}
