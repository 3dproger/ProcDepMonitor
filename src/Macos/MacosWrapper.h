#pragma once

#include "OSWrapper.hpp"

class MacosWrapper : public OSWrapper
{
public:
    struct OsxProcessInfo
    {
        int64_t pid = -1;
        int64_t parentPid = -1;
        QString name;
        QString path;
        uint32_t status = 0;
    };

    MacosWrapper();
    QList<OSProcessInfo> getProcesses(const bool includeDeps) const override;
    OSProcessInfo processByPidImpl(const int64_t pid, const bool includeDeps) const override;
    static QList<int64_t> processesPids();
    static QString getProcessName(pid_t pid);
    static QString getProcessPath(const pid_t pid);
    static bool getProcessInfo(const pid_t pid, OsxProcessInfo& result);
    static QList<OSProcessDependence> getDeps(const pid_t pid, bool& hasAccess);
    static bool revealInFinder(const QString& path);
};
