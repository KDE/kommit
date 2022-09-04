#ifndef GITKLIENTCONFIG_H
#define GITKLIENTCONFIG_H

namespace Git {
class Manager;
}
class AppConfig
{
    Git::Manager *_git;
public:
    explicit AppConfig(Git::Manager *git);

    void apply();
};

#endif // GITKLIENTCONFIG_H
