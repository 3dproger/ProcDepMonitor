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

#include "dialogselectprocess.h"
#include "ui_dialogselectprocess.h"
#include <QDebug>
#include <QMessageBox>
#include "OSWrapper.hpp"

DialogSelectProcess::DialogSelectProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSelectProcess)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog |
                   Qt::CustomizeWindowHint |
                   Qt::WindowTitleHint |
                   Qt::WindowSystemMenuHint |
                   Qt::WindowMaximizeButtonHint |
                   Qt::WindowCloseButtonHint);

    ui->tableWidgetProcesses->setColumnWidth(0, 300);
    ui->tableWidgetProcesses->setColumnWidth(1, 100);
    ui->tableWidgetProcesses->setColumnWidth(2, 100);
}

DialogSelectProcess::~DialogSelectProcess()
{
    delete ui;
}

void DialogSelectProcess::updateList()
{
    ui->tableWidgetProcesses->clearContents();
    ui->tableWidgetProcesses->setRowCount(0);

    QList<OSProcessInfo> processes = OSWrapper::instance().getProcesses();

    qDebug() << "Found processes:" << processes.count();

    for (const OSProcessInfo& info : processes)
    {
        int row = ui->tableWidgetProcesses->rowCount();
        ui->tableWidgetProcesses->insertRow(row);

        ui->tableWidgetProcesses->setItem(row, 0, new QTableWidgetItem(info.name));
        ui->tableWidgetProcesses->setItem(row, 1, new QTableWidgetItem(QString("%1").arg(info.id)));


        if (info.canGetDependencies)
        {
            ui->tableWidgetProcesses->setItem(row, 2, new QTableWidgetItem(QString("%1").arg(info.dependencies.count())));
        }
        else
        {
            ui->tableWidgetProcesses->setItem(row, 2, new QTableWidgetItem(tr("No access")));
            ui->tableWidgetProcesses->item(row, 2)->setTextColor(QColor(255, 0, 0));
        }
    }
}

void DialogSelectProcess::changeEvent(QEvent *event)
{
    QDialog::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void DialogSelectProcess::showEvent(QShowEvent *)
{
    updateList();
}

void DialogSelectProcess::on_btnCancel_clicked()
{
    _pid = -1;

    close();
}

void DialogSelectProcess::on_btnSelect_clicked()
{
    _processNameSelected = QString();

    int row = ui->tableWidgetProcesses->currentRow();

    QTableWidgetItem* itemName = ui->tableWidgetProcesses->item(row, 0);
    QTableWidgetItem* itemPid = ui->tableWidgetProcesses->item(row, 1);

    if (itemName)
    {
        _processNameSelected = itemName->text();
    }

    if (!itemPid)
    {
        QMessageBox::information(this, tr("Notification"), tr("Process not selected"));
    }
    else
    {
        bool ok;
        _pid = itemPid->text().toLongLong(&ok);

        if (!ok)
        {
            QMessageBox::critical(this, tr("Error"), tr("Internal error"));
            _pid = -1;
        }
        else
        {
            emit pidSelected(_pid);
        }

        close();
    }
}

void DialogSelectProcess::on_tableWidgetProcesses_itemDoubleClicked(QTableWidgetItem *)
{
    on_btnSelect_clicked();
}

int64_t DialogSelectProcess::pid() const
{
    return _pid;
}

QString DialogSelectProcess::processNameSelected() const
{
    return _processNameSelected;
}


void DialogSelectProcess::on_btnUpdateList_clicked()
{
    updateList();
}
