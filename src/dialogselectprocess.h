#ifndef DIALOGSELECTPROCESS_H
#define DIALOGSELECTPROCESS_H

#include <QDialog>
#include <QTableWidgetItem>

//ToDo: после нажатия сортировки по алфавиту обновление списка происходит с ошибкой

namespace Ui {
class DialogSelectProcess;
}

class DialogSelectProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSelectProcess(QWidget *parent = nullptr);
    ~DialogSelectProcess();

    QString processNameSelected() const;

    int64_t pid() const;

signals:
    void pidSelected(int64_t pid);

public slots:
    void updateList();

protected:
    void changeEvent(QEvent *event);
    void showEvent(QShowEvent *event);

private slots:
    void on_btnCancel_clicked();
    void on_btnSelect_clicked();

    void on_tableWidgetProcesses_itemDoubleClicked(QTableWidgetItem *item);

    void on_btnUpdateList_clicked();

private:
    Ui::DialogSelectProcess *ui;

    int64_t _pid = -1;
    QString _processNameSelected;
};

#endif // DIALOGSELECTPROCESS_H
