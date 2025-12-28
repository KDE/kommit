#include "statusoptions.h"

namespace Git
{

void StatusOptions::apply(git_status_options *options)
{
    options->version = GIT_STATUS_OPTIONS_VERSION;
    options->show = static_cast<git_status_show_t>(_showType);
    options->rename_threshold = _renameThreshold;
    options->flags = static_cast<unsigned int>(_options);
    if (!_baseTree.isNull())
        options->baseline = _baseTree.data();
}

StatusOptions::ShowType StatusOptions::showType() const
{
    return _showType;
}

void StatusOptions::setShowType(ShowType newShowType)
{
    _showType = newShowType;
}

Tree StatusOptions::baseTree() const
{
    return _baseTree;
}

void StatusOptions::setBaseTree(const Tree &newBaseTree)
{
    _baseTree = newBaseTree;
}

quint16 StatusOptions::renameThreshold() const
{
    return _renameThreshold;
}

void StatusOptions::setRenameThreshold(quint16 newRenameThreshold)
{
    _renameThreshold = newRenameThreshold;
}

StatusOptions::Options StatusOptions::options() const
{
    return _options;
}

void StatusOptions::setOptions(Options newOptions)
{
    _options = newOptions;
}
}
