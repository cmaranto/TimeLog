#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setName(const QString &name)
{
    ui->exportWidget->name = name;
    m_name = name;
}

void ExportDialog::setDate(const QString &date)
{
    ui->exportWidget->date = date;
    m_date = date;
    m_date.remove("/");
    m_date = m_date.simplified();
    m_date.replace( " ", "_" );
}

void ExportDialog::setTimes(const QMap<int, QStringList> &times)
{
    ui->exportWidget->times = times;
}

ExportWidget::ExportWidget(QWidget *parent) :
    QWidget(parent)
{
    m_pixmap.load(":/images/signinsheet.png");
    name = "YOUR NAME";
    date = "MM / YYYY";
}

void ExportWidget::renderFull(QPaintDevice *target)
{
    QPainter painter(target);

    painter.drawPixmap(0,0,target->width(),target->height(),m_pixmap);

    QFont font;
    font.setBold(true);
    font.setPointSize(22);
    font.setFamily("Arial");

    painter.setFont(font);

    painter.drawText(QRectF(360,105,400,50),name);
    painter.drawText(QRectF(1060,105,400,50),date);

    int xoff = 232;
    int yoff = 228;
    int bw = 128;
    int bh = 59;

    font.setPointSize(18);
    painter.setFont(font);

    int textx;
    int texty;
    QTextOption to(Qt::AlignHCenter | Qt::AlignVCenter);

    for(auto itr = times.begin(); itr != times.end(); ++itr){
        for(int i = 0; i < itr.value().size(); ++i){
            textx = xoff + (bw * i);
            texty = yoff + (bh * (itr.key() - 1));
            painter.drawText(QRectF(textx,texty,bw,bh),itr.value().at(i),to);
        }
    }
}

void ExportWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    renderFull(this);

}

void ExportDialog::on_saveButton_released()
{
    QPixmap pix(ui->exportWidget->size());
    ui->exportWidget->renderFull(&pix);
    QString fname = QString("%1_%2_log.png").arg(m_name,m_date);
    if(!pix.save(fname)){
        qDebug() << "save err" << fname;
    }
    accept();
}

