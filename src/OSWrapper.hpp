/*
 * MIT License

 * Copyright (c) 2020 Alexander Kirsanov

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * */

#ifndef OSWRAPPER_HPP
#define OSWRAPPER_HPP

#include <QList>

//Singleton class

enum SpecialDirs
{
    None,
    WindowsSystem32,
    WindowsSysWOW64,
    Windows,
    LinuxSymLinkDeleted
};

struct OSProcessDependence
{
    QString name;
    QString fileName;
    bool valid = false;
    SpecialDirs specialDir = SpecialDirs::None;

    bool operator==(const OSProcessDependence& other) const
    {
        return name == other.name &&
                fileName == other.fileName &&
                valid == other.valid &&
                specialDir == other.specialDir;
    }
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

    virtual QList<OSProcessInfo> processes();

    OSProcessInfo processByPID(int64_t pid);

    virtual ~OSWrapper() {} // In order to hide the warning

    static bool compareOSProcessInfo(const OSProcessInfo& info1, const OSProcessInfo& info2);

    static bool compareOSProcessDependence(const OSProcessDependence& dep1, const OSProcessDependence& dep2);

protected:
    OSWrapper() {}

    virtual OSProcessInfo processByPIDImpl(int64_t pid) = 0;

private:
    OSWrapper(const OSWrapper& root) = delete;
    OSWrapper& operator=(const OSWrapper&) = delete;
};

#endif // OSWRAPPER_HPP
