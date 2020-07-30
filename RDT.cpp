
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
#include <QSettings>
#include <PolylineBuilder.h>
#include <Polyline.h>

using namespace Esri::ArcGISRuntime;




RDT::RDT(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::topographic(this), this)),
    m_loggedIn(false)
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

    m_rendererModel = new RendererModel();
    setupConnections();
}

RDT::~RDT()
{
}

void RDT::addCSVLayer(QString filePath)
{        
    if (filePath.startsWith("file:///"))
        filePath = filePath.remove("file:///");

    if(filePath.endsWith("values.txt"))
    {
        addNetwork(filePath);
        return;
    }

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

    auto greenCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Light Green"), 5.0, this);
    auto greenClassBreak = new ClassBreak("Low Loss Ratio", "Loss Ratio less than 10%", 0.0, 0.1, greenCircle);
    classBreaks.append(greenClassBreak);

    auto yellowCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Yellow"), 5.0, this);
    auto yellowClassBreak = new ClassBreak("Medium Loss Ratio", "Loss Ratio Between 10% and 50%", 0.1, 0.5, yellowCircle);
    classBreaks.append(yellowClassBreak);

    auto redCircle = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("Red"), 5.0, this);
    auto classBreak = new ClassBreak("High Loss Ratio", "Loss Ratio more than 50%", 0.5, 1.0,redCircle);
    classBreaks.append(classBreak);

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
    emit startLogin(username, password);
}

void RDT::refreshJobs()
{
    if(client->isLoggedIn())
        emit startJobListUpdate("", "rWHALE-1*");
}

void RDT::getJobDetails(int index)
{
    auto jobId = m_jobsList->getJobId(index);
    emit startJobDetailsUpdate(jobId);
    return;
}

void RDT::loadResultFile(QString outputFile)
{
    //client->remoteLSCall(outputFile);

    for (auto file: m_jobDetails->getOutputs())
    {
        auto fileName =  file.toObject()["name"].toString();
        if(0 == fileName.compare(outputFile))
        {
            auto path = file.toObject()["path"].toString();
            QStringList remoteFiles;
            //TODO: we may need to handle files from different storage systems, for now using default
            remoteFiles << "system/designsafe.storage.default" + path;

            QStringList localFiles;
            m_resultsPath = QDir::tempPath() + path;
            localFiles << m_resultsPath;

            QDir(m_resultsPath.left(m_resultsPath.lastIndexOf('/'))).mkpath(".");

            client->downloadFilesCall(remoteFiles, localFiles, this);
        }
    }
}

void RDT::submitJob(QString job)
{
    auto jobDoc = QJsonDocument::fromJson(job.toUtf8());
    client->startJobCall(jobDoc.object());
}

void RDT::downloadOutputFile(QString outputFileName, QString filePath)
{
    if (filePath.startsWith("file:///"))
        filePath = filePath.remove("file:///");
    for (auto file: m_jobDetails->getOutputs())
    {
        auto fileName =  file.toObject()["name"].toString();
        if(0 == fileName.compare(outputFileName))
        {
            auto path = file.toObject()["path"].toString();
            QStringList remoteFiles;
            //TODO: we may need to handle files from different storage systems, for now using default
            remoteFiles << "system/designsafe.storage.default" + path;

            QStringList localFiles;
            localFiles << filePath;

            client->downloadFilesCall(remoteFiles, localFiles, nullptr);
        }
    }
}

void RDT::deleteLayer(int index)
{
    m_map->operationalLayers()->removeAt(index);
}

void RDT::moveLayerUp(int index)
{
    if (index > 0)
        m_map->operationalLayers()->move(index, index - 1);
}

void RDT::moveLayerDown(int index)
{
    if (index < m_map->operationalLayers()->size() - 1)
        m_map->operationalLayers()->move(index, index + 1);
}

void RDT::setRenderer(int index)
{
    auto layer = reinterpret_cast<FeatureCollectionLayer*>(m_map->operationalLayers()->at(index));
    auto table = reinterpret_cast<FeatureCollectionTable*>(layer->featureCollection()->tables()->at(0));
    m_rendererModel->setRenderer(reinterpret_cast<ClassBreaksRenderer*>(table->renderer()));
    emit rendererChanged();
}

QString RDT::username()
{
    QSettings settings("SimCenter", "Common");
    return settings.value("loginAgave", "").toString();
}

QString RDT::password()
{
    QSettings settings("SimCenter", "Common");
    return settings.value("passwordAgave", "").toString();
}

void RDT::addNetwork(QString filePath)
{

    QFile valuesFile(filePath);
    valuesFile.open(QIODevice::ReadOnly);
    auto allValues = valuesFile.readAll();
    valuesFile.close();
    auto values = allValues.split(' ');
    qDebug() << values.count();

    filePath = filePath.replace("values.txt", "seg_start.txt");
    QFile startFile(filePath);
    startFile.open(QIODevice::ReadOnly);
    QList<QPair<double, double>> startPoints;
    while(!startFile.atEnd())
    {
        auto startLine = startFile.readLine();
        startPoints.push_back(qMakePair(startLine.split(' ')[0].toDouble(), startLine.split(' ')[1].toDouble()));
    }

    qDebug() << startPoints.count();

    filePath = filePath.replace("seg_start.txt", "seg_end.txt");
    QFile endFile(filePath);
    endFile.open(QIODevice::ReadOnly);
    QList<QPair<double, double>> endPoints;
    while(!endFile.atEnd())
    {
        auto endLine = endFile.readLine();
        endPoints.push_back(qMakePair(endLine.split(' ')[0].toDouble(), endLine.split(' ')[1].toDouble()));
    }

    qDebug() << endPoints.count();


    QList<Field> fields;
    auto featureCollection = new FeatureCollection();
    fields.append(Field::createDouble("RepairRate", "Repair Rate"));

    auto featureCollectionTable = new FeatureCollectionTable(fields, GeometryType::Polyline, SpatialReference::wgs84());

    QList<Feature*> features;

    for(int i = 0; i < startPoints.count(); i++ )
    {
        auto feature = featureCollectionTable->createFeature();

        feature->attributes()->replaceAttribute("RepairRate", values[i].toDouble());

        PolylineBuilder* builder = new PolylineBuilder(SpatialReference::wgs84(), this);
        builder->addPoint(startPoints[i].first, startPoints[i].second);
        builder->addPoint(endPoints[i].first, endPoints[i].second);
        Esri::ArcGISRuntime::Polyline singleLine = builder->toPolyline();

        feature->setGeometry(singleLine);
        features.append(feature);
    }

    featureCollectionTable->addFeatures(features);
    QList<ClassBreak*> classBreaks;

    auto line1 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(254, 229, 217), 4.0, this);
    auto classbreak1 = new ClassBreak("Very Low Repair Rate", "Repair Rate less than 1e-3", 0.0, 1e-3, line1);
    classBreaks.append(classbreak1);

    auto line2 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(252, 187, 161), 4.0, this);
    auto classbreak2 = new ClassBreak("Low Repair Rate", "Repair Rate less than 0.01", 1e-3, 1e-2, line2);
    classBreaks.append(classbreak2);

    auto line3 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(252, 146, 114), 4.0, this);
    auto classbreak3 = new ClassBreak("Very Moderate Repair Rate", "Repair Rate less than 0.1", 1e-2, 1e-1, line3);
    classBreaks.append(classbreak3);

    auto line4 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(251, 106, 74), 4.0, this);
    auto classbreak4 = new ClassBreak("Moderate Repair Rate", "Repair Rate less than 1", 1e-1, 1e0, line4);
    classBreaks.append(classbreak4);

    auto line5 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(222, 45, 38), 4.0, this);
    auto classbreak5 = new ClassBreak("High Repair Rate", "Repair Rate less than 10", 1e0, 1e1, line5);
    classBreaks.append(classbreak5);

    auto line6 = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor(165, 15, 21), 4.0, this);
    auto classbreak6 = new ClassBreak("Very High Repair Rate", "Repair Rate less than 1e10", 1e0, 1e10, line6);
    classBreaks.append(classbreak6);

    auto renderer = new ClassBreaksRenderer("RepairRate", classBreaks);
    featureCollectionTable->setRenderer(renderer);


    featureCollection->tables()->append(featureCollectionTable);
    auto csvLayer = new FeatureCollectionLayer(featureCollection);
    m_map->operationalLayers()->append(csvLayer);

    Viewpoint bayArea(37.8272, -122.2913, 500000.0);
    m_mapView->setViewpoint(bayArea);

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
    });

    connect(client, &AgaveCurl::downloadFilesReturn, this, [this](bool result, QObject* sender){
        Q_UNUSED(result)
        if(sender == this)
        {
            if(m_resultsPath.endsWith(".csv"))
                this->addCSVLayer(m_resultsPath);
            else
            {
                QFile file(m_resultsPath);
                file.open(QFile::ReadOnly);
                m_textFileContents = file.readAll();
                file.close();
                emit textFileContentsChanged();
            }
        }

    });

    connect(client, &AgaveCurl::getJobListReturn, this, [this](QJsonObject jobs){
        m_jobsList->setJobs(jobs);
    });

    connect(client, &AgaveCurl::getJobDetailsReturn, this, [this](QJsonObject jobDetails){
        m_jobDetails->setJob(jobDetails);
        client->remoteLSCall(jobDetails["archivePath"].toString());
    });

    connect(client, &AgaveCurl::startJobReturn, this, [](QString jobreturn){
        qDebug() << jobreturn;//TODO: handle job submisstion result
    });

    connect(client, &AgaveCurl::loginReturn, this, [this](bool loggedIn){
        m_loggedIn = loggedIn;
        emit loggedInChanged();
    });

    connect(client, &AgaveCurl::logoutReturn, this, [this](bool loggedOut){
        m_loggedIn = !loggedOut;
        emit loggedInChanged();
    });

    connect(this, &RDT::startLogin, client, &AgaveCurl::loginCall);

    connect(this, &RDT::startJobListUpdate, client, &AgaveCurl::getJobListCall);

    connect(this, &RDT::startJobDetailsUpdate, client, &AgaveCurl::getJobDetailsCall);
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
