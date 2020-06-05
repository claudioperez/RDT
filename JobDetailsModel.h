#ifndef JOBDETAILSMODEL_H
#define JOBDETAILSMODEL_H

#include <QAbstractItemModel>
#include <QJsonObject>

class JobDetailsModel : public QAbstractItemModel
{
    Q_OBJECT
    enum JobDetailRoles{
        Name = Qt::UserRole,
        Value
    };

public:
    explicit JobDetailsModel(QObject *parent = nullptr);

signals:


    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QJsonObject m_jobDetails;
    QJsonObject m_outputs;

    QVariant getName(QModelIndex index) const;
    QVariant getValue(QModelIndex index) const;


public:
    void setJob(QJsonObject jobDetails);

};

#endif // CSVLAYER_H
