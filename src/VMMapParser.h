#pragma once

#include <QObject>

class VMMapParser
{
public:
    struct Result
    {
        bool hasAccess = false;
        int64_t pid = -1;
        QStringList regions;
    };


    static Result parseForPid(const int64_t pid);

private:
    explicit VMMapParser(){}
};
