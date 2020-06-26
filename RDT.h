
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
#include "RendererModel.h"

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
    Q_PROPERTY(JobsListModel* jobsList READ jobsList NOTIFY jobsListChanged)
    Q_PROPERTY(QStringList inputs MEMBER m_inputs NOTIFY inputsChanged)
    Q_PROPERTY(Esri::ArcGISRuntime::LayerListModel* layers READ getLayers NOTIFY layersChanged)
    Q_PROPERTY(JobDetailsModel* jobDetails READ jobDetails NOTIFY jobDetailsChanged)
    Q_PROPERTY(QByteArray textFileContents MEMBER m_textFileContents NOTIFY textFileContentsChanged)
    Q_PROPERTY(bool loggendIn MEMBER m_loggedIn NOTIFY loggedInChanged)
    Q_PROPERTY(RendererModel* renderer MEMBER m_rendererModel NOTIFY rendererChanged)

public:
    explicit RDT(QObject* parent = nullptr);
    ~RDT() override;
    Q_INVOKABLE void addCSVLayer(QString filePath);
    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool isLoggedIn();
    Q_INVOKABLE void login(QString username, QString password);
    Q_INVOKABLE void refreshJobs();
    Q_INVOKABLE void getJobDetails(int index);
    Q_INVOKABLE void loadResultFile(QString outputFile);
    Q_INVOKABLE void submitJob(QString job);
    Q_INVOKABLE void downloadOutputFile(QString outputFileName, QString filePath);
    Q_INVOKABLE void deleteLayer(int index);
    Q_INVOKABLE void moveLayerUp(int index);
    Q_INVOKABLE void moveLayerDown(int index);
    Q_INVOKABLE void setRenderer(int index);
    Q_INVOKABLE QString username();
    Q_INVOKABLE QString password();

signals:
    void mapViewChanged();
    void mapDrawStatusChanged();
    void inputsChanged();
    void layersChanged();
    void textFileContentsChanged();
    void loggedInChanged();
    void jobsListChanged();
    void jobDetailsChanged();
    void rendererChanged();
    void startLogin(QString username, QString password);
    void startJobListUpdate(QString matchingName, QString appIdFilter);
    void startJobDetailsUpdate(QString jobId);

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
    QByteArray m_textFileContents;
    bool m_loggedIn;
    RendererModel* m_rendererModel;

    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    JobsListModel* jobsList();
    bool mapDrawing() const;
    void setupConnections();
    JobDetailsModel* jobDetails();

    Esri::ArcGISRuntime::LayerListModel* getLayers();
};

#endif // RDT_H
