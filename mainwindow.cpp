#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_database(QSqlDatabase::addDatabase("QSQLITE"))

{
    ui->setupUi(this);

    ui->listView->setModel(&TimeModel::instance());
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);


    m_database.setDatabaseName("./log");
    if(!m_database.open()){
        qDebug() << "db connect failed";
    }

    connect(ui->calendarWidget,SIGNAL(selectionChanged()),this,SLOT(getTimes()));
    connect(ui->listView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onContextMenu(QPoint)));

    ui->manualTime->setDateTime(QDateTime::currentDateTime());
    getTimes();

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::addInEntry()
{
    QSqlQuery query;

    qint64 time = ui->manualTimeCheck->isChecked() ? ui->manualTime->dateTime().toSecsSinceEpoch() :
                                                     QDateTime::currentSecsSinceEpoch();

    query.prepare("INSERT INTO TIMES (timestamp_in) VALUES(:timestamp)");
    query.bindValue(":timestamp",time);
    bool res =  query.exec();
    if(!res){
        qDebug() << query.lastError();
    }else{
        m_currEntry.id = query.lastInsertId().toInt();
        m_currEntry.startTime = time;
        m_currEntry.stopTime = 0;
        TimeModel::instance().addTime(m_currEntry);        
    }
    return res;
}

bool MainWindow::addOutEntry()
{
    if(!m_currEntry.id)return false;

    QSqlQuery query;

    qint64 time = ui->manualTimeCheck->isChecked() ? ui->manualTime->dateTime().toSecsSinceEpoch() :
                                                     QDateTime::currentSecsSinceEpoch();

    query.prepare("UPDATE TIMES SET timestamp_out = :timestamp WHERE id = :id");
    query.bindValue(":timestamp",time);
    query.bindValue(":id",m_currEntry.id);
    bool res =  query.exec();
    if(!res){
        qDebug() << query.lastError();
    }else{
        m_currEntry.stopTime = time;
        TimeModel::instance().editTime(m_currEntry);
    }
    return res;
}

bool MainWindow::removeTime(const TimeEntry &te)
{
    if(!te.id)return false;

    QSqlQuery query;

    query.prepare("DELETE FROM TIMES WHERE id = :id");
    query.bindValue(":id",te.id);
    bool res =query.exec();
    if(!res){
        qDebug() << query.lastError();
    }else{
        TimeModel::instance().removeTime(te);
    }
    return res;
}

void MainWindow::getTimes()
{
    QSqlQuery query;

    QDate date = ui->calendarWidget->selectedDate();

    QDateTime start = QDateTime(date,QTime(0,0,0));
    QDateTime stop = QDateTime(date,QTime(23,59,59));

    if(ui->showMonth->isChecked()){
        start = QDateTime(QDate(date.year(),date.month(),1),QTime(0,0,0));
        stop = QDateTime(QDate(date.year(),date.month(),date.daysInMonth()),QTime(23,59,59));
    }


    query.prepare(QString("SELECT * FROM TIMES WHERE timestamp_in BETWEEN %1 AND %2")
                  .arg(start.toSecsSinceEpoch())
                  .arg(stop.toSecsSinceEpoch()));

    if(!query.exec()){
        qDebug() << "query err";
        return;
    }


    int id = query.record().indexOf("id");
    int startTime = query.record().indexOf("timestamp_in");
    int stopTime = query.record().indexOf("timestamp_out");


    TimeModel::instance().clear();
    while(query.next()){
        m_currEntry.id = query.value(id).toUInt();
        m_currEntry.startTime = query.value(startTime).toUInt();
        m_currEntry.stopTime = query.value(stopTime).toUInt();

        TimeModel::instance().addTime(m_currEntry);
    }


    ui->inOutButton->setText(!m_currEntry.id || m_currEntry.stopTime ? "IN" : "OUT");
}

void MainWindow::onContextMenu(QPoint p)
{
    QMenu menu(this);

    QAction *removeAction = menu.addAction("Remove");

    if(menu.exec(ui->listView->mapToGlobal(p)) == removeAction){
        TimeEntry te = TimeModel::instance().timeAt(ui->listView->currentIndex().row());


        qDebug() << "remove: " << te.toString();
        removeTime(te);
    }
}


void MainWindow::on_inOutButton_released()
{
    bool res = m_currEntry.startTime && !m_currEntry.stopTime  ? addOutEntry() : addInEntry();
    if(res){
        ui->inOutButton->setText(m_currEntry.stopTime ? "IN" : "OUT");
        getTimes();
    }else{
        qDebug() << "error add entry";
    }
}

void MainWindow::on_showMonth_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    getTimes();
}


void MainWindow::on_exportButton_released()
{
    QString name = QInputDialog::getText(this,"Enter Your Name","Name");
    if(name.isNull() || name.isEmpty())return;

    ExportDialog dlg(this);
    dlg.setName(name);
    dlg.setDate(ui->calendarWidget->selectedDate().toString("MM / yyyy"));

    QDate date = ui->calendarWidget->selectedDate();

    QDateTime start = QDateTime(QDate(date.year(),date.month(),1),QTime(0,0,0));
    QDateTime stop = QDateTime(QDate(date.year(),date.month(),date.daysInMonth()),QTime(23,59,59));

    QSqlQuery query;

    query.prepare(QString("SELECT * FROM TIMES WHERE timestamp_in BETWEEN %1 AND %2")
                  .arg(start.toSecsSinceEpoch())
                  .arg(stop.toSecsSinceEpoch()));

    if(!query.exec()){
        qDebug() << "query err";
        return;
    }

    int startTime = query.record().indexOf("timestamp_in");
    int stopTime = query.record().indexOf("timestamp_out");

    QMap<int,QStringList> times;
    QDateTime start_dtg,stop_dtg;
    uint stopintval;

    while(query.next()){
        start_dtg = QDateTime::fromSecsSinceEpoch(query.value(startTime).toUInt());
        stopintval = query.value(stopTime).toUInt();
        stop_dtg = QDateTime::fromSecsSinceEpoch(stopintval);

        int day = start_dtg.date().day();
        if(times.contains(day)){
            times[day].append(start_dtg.toString("hh:mm"));
            if(stopintval)times[day].append(stop_dtg.toString("hh:mm"));
        }else{
            QStringList tst = QStringList() << start_dtg.toString("hh:mm");
            if(stopintval)tst << stop_dtg.toString("hh:mm");
            times[day] = tst;
        }
    }

    if(times.isEmpty()){
        return;
    }
    dlg.setTimes(times);
    dlg.exec();

}

