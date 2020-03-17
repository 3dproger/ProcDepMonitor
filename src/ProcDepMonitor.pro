# MIT License
#
# Copyright (c) 2020 Alexander Kirsanov
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#-------------------------------------------------
#
# Project created by QtCreator 2019-08-30T00:01:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TRANSLATIONS += app_ru.ts
RC_FILE = rc.rc

#include(Utils/utils.pri)
include(I18nManager/I18nManager.pri)

unix{
    TARGET=procdepmonitor
}

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

HEADERS += \
        OSWrapper.hpp \
        dialogselectprocess.h \
        mainwindow.h


SOURCES += \
        OSWrapper.cpp \
        dialogselectprocess.cpp \
        main.cpp \
        mainwindow.cpp

unix {
HEADERS += \
        Linux/LinuxWrapper.hpp

SOURCES += \
        Linux/LinuxWrapper.cpp
}

win32 {
        LIBS += -lpsapi
        QMAKE_CXXFLAGS += "-DPSAPI_VERSION=1" # ToDo: check it
        HEADERS += \
                Windows/TaskListManager.hpp \
                Windows/ToolHelpManager.hpp \

        SOURCES += \
                Windows/ToolHelpManager.cpp \
                Windows/TaskListManager.cpp \
}

FORMS += \
        dialogselectprocess.ui \
        mainwindow.ui

RESOURCES += res.qrc

#Раздел, в котором указываем директории, в которых хранятся собранные и промежуточные файлы
CONFIG(debug, debug|release) {
    #debug
} else {
    #release - выпуск
    #В режиме выпуска необходимо так же собрать нужные библиотеки рядом с исполнительным файлом

    win32 {
        #Windows x32 and x64

        #Указываем папку, в которую будет помещён исполнительный файл с библиотеками, в зависимости от архитектуры
        contains(QT_ARCH, i386) {
            #Для Windows x32
            DESTDIR = $$_PRO_FILE_PWD_/../deploy/windows/i386/appdir
        } else {
            #Для Windows x64
            DESTDIR = $$_PRO_FILE_PWD_/../deploy/windows/amd64/appdir
        }

        #Запуск утилиты "windeployqt", которая собирает нужные dll в папку с исполнительным файлом
        #ToDo: Замечен баг в windeployqt версии Qt 5.12.4 как для x32, так и x64 версии:
        #Не копирует файлы из директории "mingw73_32/qml/Qt/labs/platform", которые нужны, если подключить модуль "Qt.labs.platform" в QML коде.
        #Поэтому эти файлы нужно скопировать вручную. Внимание: файл "qtlabsplatformplugind.dll" (с "d" на конце имени без расширения) не копировать в релизную версию,
        #так как он будет бесполезен и занимать много места
        QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt --release --qmldir $$(QTDIR)/qml $$DESTDIR

        message("Release building for Windows will be in \""$$DESTDIR"\"")
    }

    unix {
        contains(QT_ARCH, i386) {
            #Для Windows x32
            DESTDIR = $$_PRO_FILE_PWD_/../deploy/unix/i386/appdir/usr/bin
        } else {
            #Для Windows x64
            DESTDIR = $$_PRO_FILE_PWD_/../deploy/unix/amd64/appdir/usr/bin
        }

        message("Release building for Linux will be in \""$$DESTDIR"\"")
        #ToDo: реализовать вызов linuxdeployqt
    }
    macx {
        message("Building for macx")
        DESTDIR = $$_PRO_FILE_PWD_/deploy/macx#Папка, куда кладётся конечный билд
        #ToDo: реализовать вызов macdeployqt
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
