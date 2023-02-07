#ifndef TIMEMODEL_H
#define TIMEMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QDateTime>

struct TimeEntry{
    uint id = 0;
    uint startTime = 0;
    uint stopTime = 0;

    QString toString() const;
    void clear();
    bool empty() const;

};

class TimeModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TimeModel(QObject *parent = nullptr);

    static TimeModel &instance();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addTime(const TimeEntry &time);
    void editTime(const TimeEntry &time);
    void removeTime(const TimeEntry &time);
    TimeEntry timeAt(int row);
    int findTime(int id) const;
    void clear();


private:
    QList<TimeEntry> m_times;
};

#endif // TIMEMODEL_H
