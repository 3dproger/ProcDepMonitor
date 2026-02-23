#pragma once

#include "OSWrapper.hpp"

class ToolHelpManager : public OSWrapper
{
public:
    ToolHelpManager();
    QList<OSProcessInfo> getProcesses(const bool includeDeps) const override;
    bool getIsAdministrator() const;

protected:
    OSProcessInfo processByPidImpl(const int64_t pid, const bool includeDeps) const override;

private:
    static OSProcessInfo _processByPID(const int64_t pid, const bool includeDeps);
    static SpecialDirs getSpecialDir(const QString& path);
    static void addDep(QList<OSProcessDependence>& list, const OSProcessDependence& dep);
    static QString getProcessPathByPid(const int64_t pid);

    bool _isAdministrator = false;
};
