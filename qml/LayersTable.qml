import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import SimCenter.RDT 1.0

Dialog {
    property RDT rdt
    height: 360
    width: 960

    title: "Manage Layers"
    standardButtons: Dialog.Ok  | Dialog.Cancel

    property alias model: layerslistView.model
    RowLayout{
        anchors.fill: parent

    TableView
    {
        id: layerslistView

        Layout.fillWidth: true
        Layout.fillHeight: true


        TableViewColumn
        {
            title: "Name"
            role: "name"
            movable: false
            width: 180
        }


        TableViewColumn
        {
            title: "description"
            role: "description"

            movable: false
            width: 180

        }

        TableViewColumn
        {
            title: "Visible"
            movable: false
            width: 180
            delegate: CheckBox {
                checked: model.layerVisible
                onCheckedChanged: model.layerVisible = checked
            }

        }

        TableViewColumn
        {
            title: "Layer Id"
            movable: false
            width: 180
            delegate: Text {
                text: model.layerId
            }
        }

        TableViewColumn
        {
            title: "Opacity"
            movable: false
            width: 180
            delegate: Slider {
                minimumValue: 0.0
                maximumValue: 1.0
                stepSize: 0.05

                value: model.layerOpacity
                onValueChanged: model.layerOpacity = value
            }
        }

    }
    }
}
