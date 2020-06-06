
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

#include "RDT.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "FeatureCollectionLayer.h"
#include "FeatureCollection.h"
#include "FeatureCollectionTable.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "ClassBreaksRenderer.h"
#include "QJsonDocument"
#include "QDir"
#include <QUrl>
#include <QThread>

using namespace Esri::ArcGISRuntime;




RDT::RDT(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::topographic(this), this))
{
    QString tenant("https://agave.designsafe-ci.org");
    QString storage("designsafe.storage.default");

    client = new AgaveCurl(tenant, storage);
    QThread* agaveThread = new QThread();
    agaveThread->setObjectName("AgaveThread");
    client->moveToThread(agaveThread);
    connect(agaveThread, &QThread::finished, client, &QObject::deleteLater);
    connect(agaveThread, &QThread::finished, agaveThread, &QObject::deleteLater);
    agaveThread->start();

    m_jobsList = new JobsListModel();
    m_jobDetails = new JobDetailsModel();
    m_inputs << "agave://designsafe.storage.community/SimCenter/Datasets/AnchorageM7/AnchorageBuildings.zip";
    m_inputs << "agave://designsafe.storage.community/SimCenter/Datasets/AnchorageM7/AnchorageM7GMs.zip";

    setupConnections();
}

RDT::~RDT()
{
}

void RDT::addCSVLayer(QString filePath)
{
    if (filePath.startsWith("file:///"))
        filePath = filePath.remove("file:///");
    QFile csvFile(filePath);
    csvFile.open(QIODevice::ReadOnly);
    auto header = csvFile.readLine();

    QList<Field> fields;
    auto featureCollection = new FeatureCollection();
    for (auto columnName:header.split(','))
    {
        if(columnName == "LossRatio")
            fields.append(Field::createDouble(columnName, "Loss Ratio"));
        else
            fields.append(Field::createText(columnName, columnName, 16));
    }
    auto featureCollectionTable = new FeatureCollectionTable(fields, GeometryType::Point, SpatialReference::wgs84());

    QList<Feature*> features;
    while(!csvFile.atEnd())
    {
        auto line = csvFile.readLine();
        auto columns = line.trimmed().split(',');
        auto feature = featureCollectionTable->createFeature();

        if(columns.count()<9)
            continue;
        if(columns[7].isEmpty() || columns[8].isEmpty())
            continue;

        if(!columns[6].isEmpty())
            feature->attributes()->replaceAttribute("LossRatio", columns[6].toDouble());

        feature->setGeometry(Point(columns[8].toDouble(), columns[7].toDouble()));
        features.append(feature);
    }
    featureCollectionTable->addFeatures(features);


    // Define the renderer

    QList<ClassBreak*> classBreaks;
    auto redCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Red"), 5.0, this);
    auto classBreak = new ClassBreak("High Loss Ratio", "Loss Ratio more than 50%", 0.5, 1.0,redCircle);
    classBreaks.append(classBreak);

    auto greenCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Light Green"), 5.0, this);
    auto greenClassBreak = new ClassBreak("Low Loss Ratio", "Loss Ratio less than 10%", 0.0, 0.1, greenCircle);
    classBreaks.append(greenClassBreak);


    auto yellowCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Yellow"), 5.0, this);
    auto yellowClassBreak = new ClassBreak("Low Loss Ratio", "Loss Ratio less than 10%", 0.1, 0.5, yellowCircle);
    classBreaks.append(yellowClassBreak);

    auto renderer = new ClassBreaksRenderer("LossRatio", classBreaks);
    featureCollectionTable->setRenderer(renderer);

    featureCollection->tables()->append(featureCollectionTable);
    auto csvLayer = new FeatureCollectionLayer(featureCollection);
    QFileInfo fileInfo(filePath);
    csvLayer->setName(fileInfo.fileName());
    csvLayer->setDescription("Imported layer");

    m_map->operationalLayers()->append(csvLayer);
}

void RDT::refresh()
{
    m_map->load();
}

bool RDT::isLoggedIn()
{
    return client->isLoggedIn();
}

void RDT::login(QString username, QString password)
{
    client->loginCall(username, password);
}

void RDT::refreshJobs()
{
    if(client->isLoggedIn())
        client->getJobListCall("", "rWHALE-1*");
}

QString RDT::getJob(int index)
{
    auto jobId = m_jobsList->getJobId(index);
    QJsonDocument jobDetails(client->getJobDetails(jobId));
    m_jobDetails->setJob(jobDetails.object());

    client->remoteLSCall(jobDetails.object()["archivePath"].toString());

    return jobDetails.toJson(QJsonDocument::Compact);
}

void RDT::loadResults(QString path)
{
    client->remoteLSCall(path);
}

void RDT::submitJob(QString job)
{
    auto jobDoc = QJsonDocument::fromJson(job.toUtf8());
    client->startJobCall(jobDoc.object());
}

MapQuickView* RDT::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void RDT::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
    {
        return;
    }

    m_mapView = mapView;
    Viewpoint anchorage(61.2181, -149.9003, 500000.0);
    m_map->setInitialViewpoint(anchorage);

    m_mapView->setMap(m_map);


    connect(m_mapView, &MapQuickView::drawStatusChanged, this, [this](DrawStatus drawStatus)
    {
      drawStatus == DrawStatus::InProgress ? m_mapDrawing = true : m_mapDrawing = false;
      emit mapDrawStatusChanged();
    });

    emit mapViewChanged();
}

void RDT::setupConnections()
{
//    connect(m_mapView, &MapQuickView::dr, this, [this](QDropEvent* dropEvent){
//        qDebug() << dropEvent->mimeData();
    //    });

    connect(client, &AgaveCurl::remoteLSReturn, this, [this](QJsonArray remoteFiles){
        m_jobDetails->setOutputs(remoteFiles);

//        for (auto file: remoteFiles)
//        {
//            auto fileName =  file.toObject()["name"].toString();
//            if(0 == fileName.compare("RegionalDamageLoss.csv"))
//            {
//                auto path = file.toObject()["path"].toString();
//                QStringList remoteFiles;
//                remoteFiles << path;

//                QStringList localFiles;
//                m_resultsPath = QDir::tempPath() + path;
//                localFiles << m_resultsPath;

//                QDir(m_resultsPath.left(m_resultsPath.lastIndexOf('/'))).mkpath(".");

//                client->downloadFilesCall(remoteFiles, localFiles, this);

//            }
//        }
    });

    connect(client, &AgaveCurl::downloadFilesReturn, this, [this](bool result, QObject* sender){
        Q_UNUSED(result)
        if(sender == this)
        {
            this->addCSVLayer(m_resultsPath);
        }

    });

    connect(client, &AgaveCurl::getJobListReturn, this, [this](QJsonObject jobs){
        m_jobsList->setJobs(jobs);
    });

    connect(client, &AgaveCurl::startJobReturn, this, [](QString jobreturn){
        qDebug() << jobreturn;
    });
}

bool RDT::mapDrawing() const
{
    return m_mapDrawing;
}

JobsListModel *RDT::jobsList()
{
    return m_jobsList;
}

JobDetailsModel *RDT::jobDetails()
{
    return m_jobDetails;
}

LayerListModel *RDT::getLayers()
{
    return m_map->operationalLayers();
}
