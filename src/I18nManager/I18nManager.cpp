#include "I18nManager.h"
#include <QLibraryInfo>
#include <QApplication>
#include <QSettings>
#include <QDebug>


I18nManager::I18nManager()
{
    //Append available locales to _availableLocales
    LocaleInfo localeInfo;

    //Russian / ru_RU
    localeInfo = LocaleInfo();
    localeInfo.locale = QLocale::Russian;
    localeInfo.name   = firstUpperCase(localeInfo.locale.nativeLanguageName());
    localeInfo.icon   = findFlag(localeInfo.locale, IconSize::_16x16);
    _availableLocales.append(localeInfo);

    //C / en_US
    localeInfo = LocaleInfo();
    localeInfo.locale = QLocale::C;
    localeInfo.name   = "English";
    localeInfo.icon   = findFlag(localeInfo.locale, IconSize::_16x16);
    _availableLocales.append(localeInfo);

    //Load last last selected locale if possible
    QSettings settings;
    changeLocale(settings.value(_settingNameLocale, QLocale::system()).toLocale());
}

I18nManager::~I18nManager()
{
    deleteTranslators();
}

void I18nManager::changeLocalBySenderData()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
        changeLocale(action->data().toLocale());
    }
    else
    {
        qDebug("Unknown sender");
    }
}

void I18nManager::changeLocale(const QLocale &locale)
{
    QLocale().setDefault(locale);
    qDebug() << "Locale selected" << locale.name();

    //Remove previous translations
    deleteTranslators();

    if (!isAvailableLocale(locale))
    {
        qWarning() << "Locale" << locale.name() << "is unavailable (not installed)";

        QString availableLocations;
        for (int i = 0; i < _availableLocales.count(); ++i)
        {
            const LocaleInfo& localeInfo = _availableLocales.at(i);
            availableLocations += "\"" + localeInfo.locale.name() + "\"";
            if (i < _availableLocales.count() - 1)
            {
                availableLocations += ", ";
            }
        }
        qWarning("Available locales: " + availableLocations.toUtf8());

        return;
    }

    //Loading user translation files
    if (locale != QLocale::C)
    {
        _translatorApp = new QTranslator();
        if (_translatorApp->load(_userQmFilesDir + "/" + _userQmFilePrefix + locale.name()))
        {
            QApplication::installTranslator(_translatorApp);
        }
        else
        {
            qWarning("Failed to load application translation");
            delete _translatorApp;
            _translatorApp = nullptr;
        }
    }

    //Loading Qt translation files
    _translatorQt = new QTranslator();
    if (_translatorQt->load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        QApplication::installTranslator(_translatorQt);
    }
    else
    {
        qWarning("Failed to load Qt translation");
        delete _translatorQt;
        _translatorQt = nullptr;
    }

    //Searching icon file
    bool foundIcon = false;
    _currentIcon = findFlag(locale, IconSize::_16x16, &foundIcon);

    if (!foundIcon)
    {
        qWarning("Failed to load locale icon");
    }

    //Saving
    QSettings settings;
    settings.setValue(_settingNameLocale, locale);
    settings.sync();
}

QString I18nManager::firstUpperCase(QString line){
    if (line.isEmpty())
    {
        return line;
    }
    line[0] = line[0].toUpper();
    return line;
}

QIcon I18nManager::findFlag(const QLocale &locale, const IconSize& size, bool* found)
{
    QIcon icon;

    //Searching dirs
    QStringList searchPaths;

    if (locale == QLocale::C)
    {
        searchPaths = QStringList({"en_US", "en"});
    }
    else
    {
        searchPaths = QStringList({locale.name()});

        if (locale.name().length() > 2)
        {
            searchPaths.append(locale.name().left(2));
        }
    }

    //Icon size dir
    QString sizeDirName = "16x16";
    switch (size) {
    case _16x16:
        sizeDirName = "16x16";
        break;
    }

    //Merge dirNames to Path
    if (!sizeDirName.isEmpty())
    {
        for (int i = 0; i < searchPaths.count(); ++i)
        {
            searchPaths[i] = ":/data/" + searchPaths[i] + "/icons/" + sizeDirName + "/icon.png";
        }
    }

    //Attempts to open file
    bool foundIcon = false;
    for (const QString& fileName : searchPaths)
    {
        icon = QIcon(fileName);
        if (!icon.availableSizes().isEmpty())
        {
            foundIcon = true;
            break;
        }
    }

    if (found != nullptr)
    {
        *found = foundIcon;
    }


    return icon;
}

QIcon I18nManager::currentIcon()
{
    return _currentIcon;
}

QList<I18nManager::LocaleInfo> I18nManager::availableLocales()
{
    return _availableLocales;
}

bool I18nManager::isAvailableLocale(const QLocale &locale)
{
    for (const LocaleInfo& localeInfo : _availableLocales)
    {
        if (localeInfo.locale.name() == locale.name())
        {
            return true;
        }
    }
    return false;
}

QList<QAction*> I18nManager::buildActions(QObject* parent)
{
    QList<QAction*> actions;

    for (const LocaleInfo& localInfo : _availableLocales)
    {
        QAction* action = new QAction(parent);
        action->setText(localInfo.name);
        action->setIcon(localInfo.icon);
        action->setData(localInfo.locale);

        connect(action, SIGNAL(triggered()), this, SLOT(changeLocalBySenderData()));

        actions.append(action);
    }

    return actions;
}

void I18nManager::deleteTranslators()
{
    if (_translatorApp)
    {
        QApplication::removeTranslator(_translatorApp);
        delete _translatorApp;
        _translatorApp = nullptr;
    }

    if (_translatorQt)
    {
        QApplication::removeTranslator(_translatorQt);
        delete _translatorQt;
        _translatorQt = nullptr;
    }
}
