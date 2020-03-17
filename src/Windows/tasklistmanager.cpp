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

#include "TaskListManager.hpp"
#include <QDebug>
#include <QProcess>

QList<TaskListManager::Task> TaskListManager::getList()
{
    #ifndef Q_OS_WINDOWS
        qDebug() << "This class does not work on Non-Windows systems";
    #endif

    QList<Task> list;

    int exitCode = -1;

    QByteArray output = fromProcess("cmd", {"/C", "tasklist /NH /FO CSV"}, &exitCode);

    if (exitCode != 0)
    {
        qWarning() << "Exit code:" << exitCode;
    }

    output.replace("\r", "");//Remove all '\r' symbols

    QByteArrayList processOutputList = output.split('\n');

    QRegExp rx("\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\",\"(.*)\"");

    for (const QByteArray& ba : processOutputList)
    {
        if (rx.indexIn(ba) != -1)
        {
            Task task;

            //Process Name
            task.processName = rx.cap(1);

            //Process ID
            QString str = rx.cap(2);

            bool ok;
            int64_t pid = str.toLongLong(&ok);
            if (ok) { task.processId = pid; }

            //Session Name
            task.sessionName = rx.cap(3);

            //Session ID
            str = rx.cap(2);

            int64_t sid = str.toLongLong(&ok);
            if (ok) { task.sessionID = sid; }

            list.append(task);
        }
    }

    return list;
}

QByteArray TaskListManager::fromProcess(const QString &program, const QStringList &arguments, int *exitCode)
{
    QProcess process;

    process.setReadChannelMode(QProcess::MergedChannels);
    process.start(program, arguments, QProcess::ReadOnly);
    process.waitForFinished(-1);

    if (exitCode)
    {
        *exitCode = process.exitCode();
    }

    return process.readAllStandardOutput();
}
