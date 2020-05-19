#ifndef JOBSLISTMODEL_H
#define JOBSLISTMODEL_H

#include <QAbstractItemModel>
#include <QJsonObject>

struct JobDetails
{
    QString id;
    QString name;
    QString status;
    QString created;
    QString start;
    QString end;
};

class JobsListModel : public QAbstractTableModel
{
    Q_OBJECT
    enum JobRoles{
        Id = Qt::UserRole,
        Name,
        Status,
        Created,
        Start,
        End
    };
public:
    explicit JobsListModel(QObject *parent = nullptr);
    void setJobs(QJsonObject jobsJson);
    QString getJobId(int index);

signals:


    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QHash<int, QByteArray> roleNames() const override;


private:
    QList<JobDetails> m_jobs;

};

#endif // JOBSLISTMODEL_H
