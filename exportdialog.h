#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPixmap>

namespace Ui {
class ExportDialog;
}

class ExportWidget : public QWidget{
    Q_OBJECT

public:
    explicit ExportWidget(QWidget *parent = nullptr);

    QString name;
    QString date;
    QMap<int,QStringList> times;

    void renderFull(QPaintDevice *target);

private:
    QPixmap m_pixmap;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event);
};

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

    void setName(const QString &name);
    void setDate(const QString &date);
    void setTimes(const QMap<int,QStringList> &times);

private slots:
    void on_saveButton_released();

private:
    Ui::ExportDialog *ui;
    QString m_name;
    QString m_date;
};

#endif // EXPORTDIALOG_H
