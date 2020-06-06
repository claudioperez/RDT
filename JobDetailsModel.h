#ifndef JOBDETAILSMODEL_H
#define JOBDETAILSMODEL_H

#include <QAbstractItemModel>
#include <QJsonObject>
#include <QJsonArray>

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
    QJsonArray m_outputs;

    QVariant getName(QModelIndex index) const;
    QVariant getValue(QModelIndex index) const;
    QVariant getOutputType(QString filename) const;

public:
    void setJob(QJsonObject jobDetails);
    void setOutputs(QJsonArray jobDetails);

};

#endif // CSVLAYER_H
