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

    property alias model: layersListView.model
    property alias rendererModel: rendererListView.model

    GridLayout{
        anchors.fill: parent
        columns: 2

        ToolBar
        {
                Layout.fillWidth: true
                Layout.preferredHeight: 25
                Layout.columnSpan: 2

                Row
                {
                    anchors.fill: parent
                    ToolButton
                    {
                        height: parent.height
                        text: "Delete"
                        onClicked:
                        {
                            rdt.deleteLayer(layerslistView.currentRow)
                        }
                    }

                    ToolButton
                    {
                        height: parent.height
                        text: "Up"
                        onClicked:
                        {
                            rdt.moveLayerUp(layerslistView.currentRow)
                        }


                    }

                    ToolButton
                    {
                        height: parent.height
                        text: "Down"
                        onClicked:
                        {
                            rdt.moveLayerDown(layerslistView.currentRow)
                        }
                    }
                }
        }

        TableView
        {
            id: layersListView

            Layout.fillWidth: true
            Layout.fillHeight: true

            onClicked:
            {
                console.log(currentRow)
                rdt.setRenderer(currentRow)
            }
            TableViewColumn
            {
                title: "Name"
                role: "name"
                movable: false
                width: 80
            }


            TableViewColumn
            {
                title: "description"
                role: "description"

                movable: false
                width: 100

            }

            TableViewColumn
            {
                title: "Visible"
                movable: false
                width: 60
                delegate: CheckBox {
                    checked: model.layerVisible
                    onCheckedChanged: model.layerVisible = checked
                }

            }

            TableViewColumn
            {
                title: "Layer Id"
                movable: false
                width: 60
                delegate: Text {
                    text: model.layerId
                }
            }

            TableViewColumn
            {
                title: "Opacity"
                movable: false
                width: 50
                delegate: Slider {
                    minimumValue: 0.0
                    maximumValue: 1.0
                    stepSize: 0.05

                    value: model.layerOpacity
                    onValueChanged: model.layerOpacity = value
                }
            }

        }


        TableView
        {
            Layout.fillHeight:  true
            Layout.fillWidth: true

            id: rendererListView

            TableViewColumn
            {
                title: "Name"
                movable: false
                width: 80
                delegate: TextEdit
                {
                    text: model.Name


                }

            }

            TableViewColumn
            {
                title: "Description"
                role: "Description"
                movable: false
                width: 120
            }

            TableViewColumn
            {
                title: "Minimum"
                movable: false
                width: 50
                delegate: TextEdit
                {
                    text: model.Minimum

                }
            }

            TableViewColumn
            {
                title: "Maximum"
                role: "Maximum"
                movable: false
                width: 50
            }

            TableViewColumn
            {
                title: "Color"
                movable: false
                width: 50
                delegate: Rectangle
                {
                    color: model.Color
                }
            }

            TableViewColumn
            {
                title: "Shape"
                movable: false
                width: 50
                role: Shape

            }
        }

    }
}
