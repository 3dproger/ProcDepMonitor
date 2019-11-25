#ifndef OSWRAPPER_HPP
#define OSWRAPPER_HPP

#include <QList>

//Singleton class

enum SpecialDirs
{
    None,
    WindowsSystem32,
    WindowsSysWOW64,
    Windows
};

struct OSProcessDependence
{
    QString name;
    QString fileName;
    bool valid = false;
    SpecialDirs specialDir = SpecialDirs::None;
};

struct OSProcessInfo
{
    QString name;
    QString fileName;
    int64_t id = -1;
    bool valid = false;
    bool canGetDependencies = false;

    QList<OSProcessDependence> dependencies;
};

class OSWrapper
{
public:
    static OSWrapper& instance();
    virtual QList<OSProcessInfo> getProcesses();

    virtual OSProcessInfo processByPID(int64_t pid);

    virtual ~OSWrapper() {} // In order to hide the warning

    static bool sortOSProcessInfo(const OSProcessInfo& info1, const OSProcessInfo& info2);

    static bool sortOSProcessDependence(const OSProcessDependence& dep1, const OSProcessDependence& dep2);

protected:
    OSWrapper() {}

private:
    OSWrapper(const OSWrapper& root) = delete;
    OSWrapper& operator=(const OSWrapper&) = delete;
};

#endif // OSWRAPPER_HPP
