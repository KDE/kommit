#pragma once

#include <QString>

#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT AddSubmoduleOptions
{
public:
    AddSubmoduleOptions();

    QString url;
    QString path;
};

}
