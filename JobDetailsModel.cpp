#include "JobDetailsModel.h"
#include "QJsonArray"


JobDetailsModel::JobDetailsModel(QObject *parent) : QAbstractItemModel(parent)
{

}


QModelIndex JobDetailsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return createIndex(row, column, nullptr);
    else if(parent.row() == 10)
        return createIndex(row, column, (void*)&m_jobDetails);
    else if(parent.row() == 11)
        return createIndex(row, column, (void*)&m_outputs);

    return createIndex(row, column, nullptr);
}

QModelIndex JobDetailsModel::parent(const QModelIndex &child) const
{
    if(child.isValid())
    {
        if(child.internalPointer() == &m_jobDetails)
            return index(10, 0, QModelIndex());
        else if(child.internalPointer() == &m_outputs)
            return index(11, 0, QModelIndex());
    }

    return QModelIndex();
}

int JobDetailsModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return 12;
    else if(parent.row() == 10 && !m_jobDetails.isEmpty())
        return m_jobDetails["inputs"]["dataFiles"].toArray().size();
    else if(parent.row() == 11)
        return m_outputs.size();

    return 0;
}

int JobDetailsModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid() || parent.row() == 10 || parent.row() == 11)
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
        {
            if(index.internalPointer() == &m_jobDetails)
                return QString::number(index.row());
            else if(index.internalPointer() == &m_outputs)
                return getOutputType(m_outputs[index.row()]["name"].toString());

        }
        return getName(index);
    }
    else if(role == JobDetailRoles::Value)
    {
        if(index.parent() != QModelIndex())
        {
            if(index.internalPointer() == &m_jobDetails)
                return m_jobDetails["inputs"]["dataFiles"][index.row()];
            else if(index.internalPointer() == &m_outputs)
                return m_outputs[index.row()]["name"];

        }

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

void JobDetailsModel::setOutputs(QJsonArray outputs)
{
    if (outputs.size() == 0)
        return;

    if (0 == outputs[0].toObject()["name"].toString().compare("."))
        outputs.removeAt(0);

    beginResetModel();
    m_outputs = outputs;
    endResetModel();
}

QJsonArray JobDetailsModel::getOutputs()
{
    return m_outputs;
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

QVariant JobDetailsModel::getOutputType(QString filename) const
{
    if(0 == filename.compare("RegionalDamageLoss.csv"))
        return "Results";
    else if(0 == filename.compare("logs.zip"))
        return "Simulation Logs";
    else if(filename.endsWith(".out"))
        return "Output File";
    else if(filename.endsWith(".err"))
        return "Error File";
    else if(0 == filename.compare(".agave.log"))
        return "Agave Log";
    else if(filename.contains("WorkflowTasks"))
        return "Simulation Tasks";
    else
        return QVariant();
}
