#pragma once

#include <git2/status.h>

#include <Kommit/Tree>

namespace Git
{

class StatusOptions
{
public:
    enum ShowType {
        IndexAndWorkdir = GIT_STATUS_SHOW_INDEX_AND_WORKDIR,
        IndexOnly = GIT_STATUS_SHOW_INDEX_ONLY,
        WorkdirOnly = GIT_STATUS_SHOW_WORKDIR_ONLY
    };

    enum Options {
        IncludeUntracked = GIT_STATUS_OPT_INCLUDE_UNTRACKED,
        IncludeIgnored = GIT_STATUS_OPT_INCLUDE_IGNORED,
        IncludeUnmodified = GIT_STATUS_OPT_INCLUDE_UNMODIFIED,
        ExcludeSubmodules = GIT_STATUS_OPT_EXCLUDE_SUBMODULES,
        RecurseUntrackedDirs = GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS,
        DisablePathspecMatch = GIT_STATUS_OPT_DISABLE_PATHSPEC_MATCH,
        RecurseIgnoredDirs = GIT_STATUS_OPT_RECURSE_IGNORED_DIRS,
        RenamesHeadToIndex = GIT_STATUS_OPT_RENAMES_HEAD_TO_INDEX,
        RenamesIndexToWorkdir = GIT_STATUS_OPT_RENAMES_INDEX_TO_WORKDIR,
        SortCaseSensitively = GIT_STATUS_OPT_SORT_CASE_SENSITIVELY,
        SortCaseInsensitively = GIT_STATUS_OPT_SORT_CASE_INSENSITIVELY,
        RenamesFromRewrites = GIT_STATUS_OPT_RENAMES_FROM_REWRITES,
        NoRefresh = GIT_STATUS_OPT_NO_REFRESH,
        UpdateIndex = GIT_STATUS_OPT_UPDATE_INDEX,
        IncludeUnreadable = GIT_STATUS_OPT_INCLUDE_UNREADABLE,
        IncludeUnreadableAsUntracked = GIT_STATUS_OPT_INCLUDE_UNREADABLE_AS_UNTRACKED,
    };

    void apply(git_status_options *options);

    ShowType showType() const;
    void setShowType(ShowType newShowType);

    Tree baseTree() const;
    void setBaseTree(const Tree &newBaseTree);

    quint16 renameThreshold() const;
    void setRenameThreshold(quint16 newRenameThreshold);

    Options options() const;
    void setOptions(Options newOptions);

private:
    ShowType _showType;
    Tree _baseTree;
    quint16 _renameThreshold{50};
    Options _options{static_cast<Options>(GIT_STATUS_OPT_DEFAULTS)};
};

}
