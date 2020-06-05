
// Copyright 2019 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#ifndef RDT_H
#define RDT_H
#include <QObject>
#include <QAbstractTableModel>
#include "AgaveCurl.h"
#include "JobsListModel.h"
#include "LayerListModel.h"
#include "JobDetailsModel.h"

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    int rowCount(const QModelIndex & = QModelIndex()) const override
    {
        return 1;
    }

    int columnCount(const QModelIndex & = QModelIndex()) const override
    {
        return 2;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        switch (role) {
            case Qt::DisplayRole:
                if (index.column() == 0)
                    return QString("Layer");
                else
                    return QString("Symbol Size");
            default:
                break;
        }

        return QVariant();
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return { {Qt::DisplayRole, "display"} };
    }
};

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QObject>

class RDT : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(bool mapDrawing READ mapDrawing NOTIFY mapDrawStatusChanged)
    Q_PROPERTY(JobsListModel* jobsList READ jobsList)
    Q_PROPERTY(QStringList inputs MEMBER m_inputs NOTIFY inputsChanged)
    Q_PROPERTY(Esri::ArcGISRuntime::LayerListModel* layers READ getLayers NOTIFY layersChanged)
    Q_PROPERTY(JobDetailsModel* jobDetails READ jobDetails)


public:
    explicit RDT(QObject* parent = nullptr);
    ~RDT() override;
    Q_INVOKABLE void addCSVLayer(QString filePath);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool isLoggedIn();
    Q_INVOKABLE void login(QString username, QString password);
    Q_INVOKABLE void refreshJobs();
    Q_INVOKABLE QString getJob(int index);
    Q_INVOKABLE void loadResults(QString path);
    Q_INVOKABLE void submitJob(QString job);

signals:
    void mapViewChanged();
    void mapDrawStatusChanged();
    void inputsChanged();
    void layersChanged();


private:
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    JobsListModel* m_jobsList = nullptr;
    bool m_mapDrawing = false;
    AgaveCurl* client;
    QString m_resultsPath;
    QStringList m_inputs;
    JobDetailsModel* m_jobDetails;

    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    JobsListModel* jobsList();
    bool mapDrawing() const;
    void setupConnections();
    JobDetailsModel* jobDetails();

    Esri::ArcGISRuntime::LayerListModel* getLayers();
};

#endif // RDT_H
