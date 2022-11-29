#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>
#include <QSqlError>
#include <QSqlRecord>
#include <QInputDialog>

#include "timemodel.h"
#include "exportdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool addInEntry();
    bool addOutEntry();

public slots:
    void getTimes();

private slots:
    void on_inOutButton_released();

    void on_showMonth_stateChanged(int arg1);

    void on_exportButton_released();

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_database;
    TimeEntry m_currEntry;
};
#endif // MAINWINDOW_H
