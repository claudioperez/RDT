#include "RendererModel.h"
#include "SimpleMarkerSymbol.h"

RendererModel::RendererModel()
{

}

void RendererModel::setRenderer(Esri::ArcGISRuntime::ClassBreaksRenderer *renderer)
{
    beginResetModel();
    m_renderer = renderer;
    endResetModel();
}


int RendererModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid() || !m_renderer)
        return 0;

    return m_renderer->classBreaks()->rowCount();
}

QVariant RendererModel::data(const QModelIndex &index, int role) const
{
    if (!m_renderer)
        return QVariant();

    switch(role)
    {
        case RendererRoles::Name:
            return m_renderer->classBreaks()->at(index.row())->label();
        case RendererRoles::Description:
            return m_renderer->classBreaks()->at(index.row())->description();
        case RendererRoles::Minimum:
            return m_renderer->classBreaks()->at(index.row())->minValue();
        case RendererRoles::Maximum:
            return m_renderer->classBreaks()->at(index.row())->maxValue();
        case RendererRoles::Color:
            return reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->color();
        case RendererRoles::Shape:
            return static_cast<int>(reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->style());
        case RendererRoles::Size:
            return reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->size();
    }


    return QVariant();
}

QHash<int, QByteArray> RendererModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[RendererRoles::Name] = "Name";
    roles[RendererRoles::Description] = "Description";
    roles[RendererRoles::Minimum] = "Minimum";
    roles[RendererRoles::Maximum] = "Maximum";
    roles[RendererRoles::Color] = "Color";
    roles[RendererRoles::Size] = "Size";
    roles[RendererRoles::Shape] = "Shape";

    return roles;
}


bool RendererModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_renderer)
        return false;

    switch(role)
    {
        case RendererRoles::Name:
        {
            m_renderer->classBreaks()->at(index.row())->setLabel(value.toString());
            break;
        }
        case RendererRoles::Description:
        {
            m_renderer->classBreaks()->at(index.row())->setDescription(value.toString());
            break;
        }
        case RendererRoles::Minimum:
        {
            m_renderer->classBreaks()->at(index.row())->setMinValue(value.toDouble());
            break;
        }
        case RendererRoles::Maximum:
        {
            m_renderer->classBreaks()->at(index.row())->setMaxValue(value.toDouble());
            break;
        }
        case RendererRoles::Color:
        {
            reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->setColor(value.value<QColor>());
            break;
        }
        case RendererRoles::Size:
        {
            reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->setSize(value.toFloat());
            break;
        }
        case RendererRoles::Shape:
        {
            reinterpret_cast<Esri::ArcGISRuntime::SimpleMarkerSymbol*>(m_renderer->classBreaks()->at(index.row())->symbol())->setStyle(
                        static_cast<Esri::ArcGISRuntime::SimpleMarkerSymbolStyle>(value.toInt()));
            break;
        }
    }

    emit dataChanged(index, index, {role});
    return true;
}

Qt::ItemFlags RendererModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;

    return QAbstractListModel::flags(index);
}
