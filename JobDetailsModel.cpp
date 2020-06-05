#include "JobDetailsModel.h"
#include "QJsonArray"

JobDetailsModel::JobDetailsModel(QObject *parent) : QAbstractItemModel(parent)
{

}


QModelIndex JobDetailsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, nullptr);
    else
        return createIndex(row, column, (void*)&m_outputs);
}

QModelIndex JobDetailsModel::parent(const QModelIndex &child) const
{
    if(child.isValid() && child.internalPointer() == &m_outputs)
        return index(10, 0, QModelIndex());

    return QModelIndex();
}

int JobDetailsModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return 11;

    if(parent.row() == 10)
        return m_outputs.isEmpty()?0:m_outputs["result"].toArray().size();

    return 0;
}

int JobDetailsModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return 1;

    if(parent.row() == 10)
        return 1;

    return 0;
}

QVariant JobDetailsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if (role == JobDetailRoles::Name)
    {
        if(index.parent() != QModelIndex())
            return "outputFile";
        return getName(index);
    }
    else if(role == JobDetailRoles::Value)
    {
        if(index.parent() != QModelIndex())
            return "outputName";
        return getValue(index);
    }
    return QVariant();
}

void JobDetailsModel::setJob(QJsonObject jobDetails)
{
    beginResetModel();
    m_jobDetails = jobDetails;
    endResetModel();
}


QHash<int, QByteArray> JobDetailsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[JobDetailRoles::Name] = "Name";
    roles[JobDetailRoles::Value] = "Value";

    return roles;
}

QVariant JobDetailsModel::getName(QModelIndex index) const
{
    static QMap<int, QVariant> nameMap
    {
        {0, "Name"},
        {1, "Id"},
        {2, "App Id"},
        {3, "Submitted"},
        {4, "Started"},
        {5, "Ended"},
        {6, "Nodes Count"},
        {7, "Buildings Count"},
        {8, "Configuration"},
        {9, "Processors Per Node"},
        {10, "Inputs"},
        {11, "Outputs"}
    };

    if(nameMap.find(index.row()) != nameMap.end())
        return nameMap[index.row()];

    return QVariant();


}

QVariant JobDetailsModel::getValue(QModelIndex index) const
{
    if(m_jobDetails.isEmpty())
        return QVariant();

    switch (index.row()) {
        case 0:
            return m_jobDetails["name"];
        case 1:
            return m_jobDetails["id"];
        case 2:
            return m_jobDetails["appId"];
        case 3:
            return m_jobDetails["created"];
        case 4:
            return m_jobDetails["remoteStarted"];
        case 5:
            return m_jobDetails["ended"];
        case 6:
            return m_jobDetails["nodeCount"];
        case 7:
            return m_jobDetails["parameters"]["buildingsCount"];
        case 8:
            return m_jobDetails["inputs"]["configFile"];
        case 9:
            return m_jobDetails["processorsPerNode"];
        default:
            break;
    }

    return QVariant();
}
