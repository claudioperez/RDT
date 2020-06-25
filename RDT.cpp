
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
    client->loginCall(username, password);
}

void RDT::refreshJobs()
{
    if(client->isLoggedIn())
        client->getJobListCall("", "rWHALE-1*");
}

void RDT::getJobDetails(int index)
{
    auto jobId = m_jobsList->getJobId(index);
    client->getJobDetailsCall(jobId);
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
