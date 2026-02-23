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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "QMessageBox"
#include <QClipboard>
#include <QProcess>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDir>
#include <QUrl>
#include <QOperatingSystemVersion>

#ifdef Q_OS_MAC
#include "Macos/MacosWrapper.h"
#endif

namespace
{
QString getShowInFMText()
{
    if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::Windows)
    {
        return QTranslator::tr("Show in Explorer");
    }
    else if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::MacOS)
    {
        return QTranslator::tr("Reveal in Finder");
    }

    return QTranslator::tr("Show in File Manager");
}
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->btnShowInExplorer->setText(getShowInFMText());

    //Window icon (for Linux)
    setWindowIcon(QIcon(":/res/logo.svg"));

    ui->tableWidgetResult->setColumnWidth(0, 200);
    ui->tableWidgetResult->setColumnWidth(1, 450);

    ui->tableWidgetResult->setContextMenuPolicy(Qt::CustomContextMenu);

    _itemContextMenu = new QMenu(ui->tableWidgetResult);

    QAction* action;

    action = new QAction(getShowInFMText(), _itemContextMenu);

    connect(action, &QAction::triggered, this, &MainWindow::on_btnShowInExplorer_clicked);
    _itemContextMenu->addAction(action);

    _itemContextMenu->addSeparator();

    action = new QAction(tr("Copy Path"), _itemContextMenu);
    connect(action, &QAction::triggered, this, &MainWindow::copyPath);
    _itemContextMenu->addAction(action);

    action = new QAction(tr("Copy Name"), _itemContextMenu);
    connect(action, &QAction::triggered, this, &MainWindow::copyName);
    _itemContextMenu->addAction(action);


    connect(ui->actionShow_only_existing_objects, &QAction::triggered, this, &MainWindow::updateDepsList);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_actionAboutSoftware_triggered()
{
    QString text =
            QCoreApplication::applicationName() + " " + QCoreApplication::applicationVersion() + "\n" +
            tr("Author: %1").arg(QCoreApplication::organizationName()) + "\n" +
            QCoreApplication::organizationDomain() + "\n" +
            tr("Qt: %1 %2").arg(qVersion()).arg(QSysInfo::buildCpuArchitecture()) + "\n" +
            tr("Build Date and Time: %1 %2").arg(__DATE__).arg(__TIME__);

    QMessageBox::about(this, tr("About Software"), text);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::changeEvent(QEvent *event){
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        qDebug("Language changed");
    }
}

void MainWindow::on_btnSelectProcess_clicked()
{
    if (!_dialogSelectProcess)
    {
        _dialogSelectProcess = new DialogSelectProcess(this);
        _dialogSelectProcess->setModal(true);
        connect(_dialogSelectProcess, &DialogSelectProcess::pidSelected, this, &MainWindow::onPidSelected);
    }

    _dialogSelectProcess->show();
}

void MainWindow::onPidSelected(int64_t pid)
{
    _pid = pid;

    ui->tableWidgetResult->clearContents();
    ui->tableWidgetResult->setRowCount(0);

    const OSProcessInfo info = OSWrapper::instance().getProcessByPid(pid, true);

    qDebug() << "Process ID:" << info.id << ", Process Name:" << info.name;

    setWindowTitle(
                QString("%1 - [%2 (%3)]")
                .arg(QApplication::applicationName())
                .arg(info.name)
                .arg(info.id));

    for (const OSProcessDependence& dep : info.deps)
    {
        if (ui->actionShow_only_existing_objects->isChecked() && !dep.info.exists())
        {
            continue;
        }

        int row = ui->tableWidgetResult->rowCount();

        ui->tableWidgetResult->insertRow(row);


        QTableWidgetItem* itemName = new QTableWidgetItem(dep.name);
        QTableWidgetItem* itemPath = new QTableWidgetItem(dep.fileName);

        QColor textColor;

        switch (dep.specialDir) {
        case SpecialDirs::WindowsSystem32:
            textColor = QColor(0, 0, 255);
            break;
        case SpecialDirs::WindowsSysWOW64:
            textColor = QColor(128, 0, 190);
            break;
        case SpecialDirs::Windows:
            textColor = QColor(128, 64, 128);
            break;
        case SpecialDirs::LinuxSymLinkDeleted:
            textColor = QColor(255, 0, 255);
            break;
        case SpecialDirs::None:
            break;
        }

        if (!dep.valid){
            textColor = QColor(255, 0, 0);
            if (itemName->text().isEmpty()) { itemName->setText(tr("<unknown>")); }
            if (itemPath->text().isEmpty()) { itemPath->setText(tr("<unknown>")); }
        }

        if (textColor.isValid())
        {
            itemName->setForeground(textColor);
            itemPath->setForeground(textColor);
        }

        if (dep.executable)
        {
            itemName->setBackground(QColor(255, 224, 178));
            itemPath->setBackground(QColor(255, 224, 178));
        }

        ui->tableWidgetResult->setItem(row, 0, itemName);
        ui->tableWidgetResult->setItem(row, 1, itemPath);
    }

    ui->btnShowInExplorer->setEnabled(ui->tableWidgetResult->currentRow() != -1);
    ui->btnUpdateList->setEnabled(pid != -1);
}

void MainWindow::on_btnShowInExplorer_clicked()
{
    int row = ui->tableWidgetResult->currentRow();

    QTableWidgetItem* item = ui->tableWidgetResult->item(row, 1);

    if (item && !item->text().isEmpty())
    {
        if (!showInGraphicalShell(item->text()))
        {
            QMessageBox::critical(this, tr("Error"), tr("Failed to open path \"%1\"").arg(item->text()));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Internal error"));
    }
}

void MainWindow::on_tableWidgetResult_currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)
{
    ui->btnShowInExplorer->setEnabled(ui->tableWidgetResult->currentRow() != -1);
}

void MainWindow::on_tableWidgetResult_itemDoubleClicked(QTableWidgetItem *)
{
    if (ui->btnShowInExplorer->isEnabled())
    {
        on_btnShowInExplorer_clicked();
    }
}

void MainWindow::on_tableWidgetResult_customContextMenuRequested(const QPoint &)
{
    if (_itemContextMenu)
    {
        _itemContextMenu->setEnabled(ui->btnShowInExplorer->isEnabled());

        _itemContextMenu->exec(QCursor::pos());
    }
}

void MainWindow::copyPath()
{
    int row = ui->tableWidgetResult->currentRow();

    QTableWidgetItem* item = ui->tableWidgetResult->item(row, 1);

    if (item && !item->text().isEmpty())
    {
        QClipboard *p_Clipboard = QApplication::clipboard();
        p_Clipboard->setText(item->text());
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Internal error"));
    }
}

void MainWindow::copyName()
{
    int row = ui->tableWidgetResult->currentRow();

    QTableWidgetItem* item = ui->tableWidgetResult->item(row, 0);

    if (item && !item->text().isEmpty())
    {
        QClipboard *p_Clipboard = QApplication::clipboard();
        p_Clipboard->setText(item->text());
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Internal error"));
    }
}

void MainWindow::on_btnUpdateList_clicked()
{
    updateDepsList();
}

bool MainWindow::showInGraphicalShell(const QString &path)
{
    QFileInfo fileInfo(path);

#if defined(Q_OS_WIN)
    if (fileInfo.isFile())
    {
        QStringList arguments = {"/select,", QDir::toNativeSeparators(path)};
        if (QProcess::startDetached("explorer.exe", arguments))
        {
            return true;
        }
        else
        {
            qWarning() << "Failed to execute script";
        }
    }
#elif defined(Q_OS_MACOS)
    if (MacosWrapper::revealInFinder(path))
    {
        return true;
    }
    else
    {
        qWarning() << "Failed reveal in Finder";
    }
#endif

    QUrl url;

    if (fileInfo.isFile() || !fileInfo.exists())
    {
        url = QUrl::fromLocalFile(fileInfo.path());
    }
    else
    {
        url = QUrl::fromLocalFile(path);
    }

    if (!QDesktopServices::openUrl(url))
    {
        qCritical() << "Failed to open url:" << url;
        return false;
    }

    return true;
}

void MainWindow::updateDepsList()
{
    onPidSelected(_pid);
}
