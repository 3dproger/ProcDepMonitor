/*
 * MIT License

 * Copyright (c) 2020-2026 Alexander Kirsanov

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

#pragma once

#include "OSWrapper.hpp"

class MacosWrapper : public OSWrapper
{
public:
    MacosWrapper();
    QList<OSProcessInfo> getProcesses(const bool includeDeps) const override;
    OSProcessInfo processByPidImpl(const int64_t pid, const bool includeDeps) const override;
    static QList<int64_t> processesPids();
    static QString getProcessName(pid_t pid);
    static QString getProcessPath(const pid_t pid);
    static QList<OSProcessDependence> getDeps(const pid_t pid, bool& hasAccess);
    static bool revealInFinder(const QString& path);
};
