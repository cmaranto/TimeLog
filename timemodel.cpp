#include "timemodel.h"

TimeModel::TimeModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

TimeModel &TimeModel::instance()
{
    static TimeModel model;
    return model;
}

int TimeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_times.size();
}

QVariant TimeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    return m_times.at(index.row()).toString();
}

void TimeModel::addTime(const TimeEntry &time)
{
    beginInsertRows(QModelIndex(),m_times.size(),m_times.size());
    m_times << time;
    endInsertRows();
}

void TimeModel::editTime(const TimeEntry &time)
{
    int row = findTime(time.id);
    if(row){
        m_times[row] = time;
        emit dataChanged(index(row),index(row));
    }
}

int TimeModel::findTime(int id) const
{
    for(int i = 0; i < m_times.size(); ++i){
        if(m_times.at(i).id == id){
            return i;
        }
    }
    return 0;
}

void TimeModel::clear()
{
    beginRemoveRows(QModelIndex(),0,m_times.size());
    m_times.clear();
    endRemoveRows();
}



QString TimeEntry::toString() const
{
    QString startDate = QDateTime::fromSecsSinceEpoch(startTime).toString("MM/dd/yyyy hh:mm:ss");
    QString stopDate = stopTime ? QDateTime::fromSecsSinceEpoch(stopTime).toString("MM/dd/yyyy hh:mm:ss") : " ";
    return QString("%1 - %2").arg(startDate,stopDate);
}

void TimeEntry::clear()
{
    id = 0;
    startTime = 0;
    stopTime = 0;
}

bool TimeEntry::empty() const
{
    return !startTime && !stopTime;
}
