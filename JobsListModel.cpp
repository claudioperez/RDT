#include "JobsListModel.h"
#include "QJsonArray"

JobsListModel::JobsListModel(QObject *parent) : QAbstractTableModel(parent)
{

}


int JobsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_jobs.size();
}

int JobsListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 6;
}

QVariant JobsListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case JobRoles::Name:
            return m_jobs[index.row()].name;
            break;
        case JobRoles::Status:
            return m_jobs[index.row()].status;
            break;
        case JobRoles::Created:
            return m_jobs[index.row()].created;
            break;
        case JobRoles::Start:
            return m_jobs[index.row()].start;
            break;
        case JobRoles::End:
            return m_jobs[index.row()].end;
            break;
        case JobRoles::Id:
            return m_jobs[index.row()].id;
            break;
    }

    if(role != Qt::DisplayRole)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        switch(index.column())
        {
            case 0:
                return m_jobs[index.row()].name;
                break;
            case 1:
                return m_jobs[index.row()].status;
                break;
            case 2:
                return m_jobs[index.row()].created;
                break;
            case 3:
                return m_jobs[index.row()].start;
                break;
            case 4:
                return m_jobs[index.row()].end;
                break;
            case 5:
                return m_jobs[index.row()].id;
                break;
        }
    }


    return QString("N/A");
}

void JobsListModel::setJobs(QJsonObject jobsJson)
{
    beginResetModel();
    m_jobs.clear();
    if(!jobsJson.keys().contains("jobs"))
        return;


    for(auto jobJson: jobsJson["jobs"].toArray())
    {
        JobDetails job;

        job.id = jobJson.toObject()["id"].toString();
        job.name = jobJson.toObject()["name"].toString();
        job.start = jobJson.toObject()["remoteStarted"].toString();
        job.end = jobJson.toObject()["ended"].toString();
        job.status = jobJson.toObject()["status"].toString();
        job.created = jobJson.toObject()["created"].toString();
        m_jobs.push_back(job);
    }
    endResetModel();
}

QString JobsListModel::getJobId(int index)
{
    return m_jobs[index].id;
}


QVariant JobsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return "";
}


QHash<int, QByteArray> JobsListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[JobRoles::Id] = "Id";
    roles[JobRoles::Name] = "Name";
    roles[JobRoles::Status] = "Status";
    roles[JobRoles::Created] = "Created";
    roles[JobRoles::Start] = "Start";
    roles[JobRoles::End] = "End";

    return roles;
}
