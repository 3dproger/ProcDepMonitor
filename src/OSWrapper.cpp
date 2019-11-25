#include "OSWrapper.hpp"
#ifdef Q_OS_WIN32
#include "Windows/Win32Wrapper.hpp"
#endif

OSWrapper &OSWrapper::instance()
{
    //It is signleton
    #ifdef Q_OS_WIN32
    static Win32Wrapper instance;
    #else
    qFatal("This OS is not yet supported");
    static OSWrapper instance;
    #endif

    return instance;
}

QList<OSProcessInfo> OSWrapper::getProcesses()
{
    return QList<OSProcessInfo>();
}

OSProcessInfo OSWrapper::processByPID(int64_t)
{
    qDebug("the function processByPID(int64_t pid) must be overridden in the heir classes!");
    return OSProcessInfo();
}

bool OSWrapper::sortOSProcessInfo(const OSProcessInfo &info1, const OSProcessInfo &info2)
{
    if (info1.canGetDependencies && !info2.canGetDependencies)
    {
        return true;
    }
    else if (!info1.canGetDependencies && info2.canGetDependencies)
    {
        return false;
    }
    else if (!info1.canGetDependencies && !info2.canGetDependencies)
    {
        return info1.id < info2.id;
    }

    #ifdef Q_OS_WIN32
        return info1.name.toLower() < info2.name.toLower();
    #else
        return info1.name < info2.name;
    #endif
}

bool OSWrapper::sortOSProcessDependence(const OSProcessDependence &dep1, const OSProcessDependence &dep2)
{
    if (dep1.specialDir != dep2.specialDir)
    {
        return dep1.specialDir < dep2.specialDir;
    }

    #ifdef Q_OS_WIN32
        return dep1.name.toLower() < dep2.name.toLower();
    #else
        return dep1.name < dep2.name;
    #endif
}
