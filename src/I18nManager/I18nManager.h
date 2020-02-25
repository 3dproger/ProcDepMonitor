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

#ifndef I18NMANAGER_H
#define I18NMANAGER_H

#include <QTranslator>
#include <QIcon>
#include <QLocale>
#include <QAction>

class I18nManager : public QObject
{
    Q_OBJECT

    struct LocaleInfo{
        QIcon icon;
        QString name;
        QLocale locale;
    };

    enum IconSize {_16x16};

public:
    I18nManager();
    ~I18nManager();
    void changeLocale(const QLocale& locale);
    QIcon findFlag(const QLocale& locale, const IconSize& size, bool* found = nullptr);
    QIcon currentIcon();
    QList<LocaleInfo> availableLocales();
    bool isAvailableLocale(const QLocale& locale);
    QList<QAction*> buildActions(QObject* parent = nullptr);

private slots:
    void changeLocalBySenderData();

private:
    void deleteTranslators();
    QString firstUpperCase(QString line);

    QTranslator* _translatorQt   = nullptr;//Translates standard Qt strings
    QTranslator* _translatorApp = nullptr;//Translates user application strings

    QIcon _currentIcon;

    QString _settingNameLocale = QString(typeid (*this).name()) + "/last";//Where to save last selected locale
    QString _userQmFilesDir = ":";
    QString _userQmFilePrefix = "app_";

    QList<LocaleInfo> _availableLocales;
};

#endif // I18NMANAGER_H
