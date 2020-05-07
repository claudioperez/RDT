
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

#include <QUrl>

using namespace Esri::ArcGISRuntime;




RDT::RDT(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::topographic(this), this))
{
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
    m_map->operationalLayers()->append(csvLayer);
}

void RDT::refresh()
{
    m_map->load();
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
}

bool RDT::mapDrawing() const
{
    return m_mapDrawing;
}
