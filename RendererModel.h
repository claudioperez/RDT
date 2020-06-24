#ifndef RENDERERMODEL_H
#define RENDERERMODEL_H

#include <QAbstractListModel>
#include <ClassBreaksRenderer.h>

Q_DECLARE_METATYPE(Esri::ArcGISRuntime::SimpleMarkerSymbolStyle)

class RendererModel : public QAbstractListModel
{
    Q_OBJECT

    enum RendererRoles{
        Name = Qt::UserRole,
        Description,
        Minimum,
        Maximum,
        Color,
        Size,
        Shape
    };

public:
    RendererModel();
    void setRenderer(Esri::ArcGISRuntime::ClassBreaksRenderer* renderer);

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    Esri::ArcGISRuntime::ClassBreaksRenderer* m_renderer = nullptr;



};

#endif // RENDERERMODEL_H
