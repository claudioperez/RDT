import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import SimCenter.RDT 1.0

Dialog {
    title: "Job Manager"
    standardButtons: Dialog.Ok
    height: 480
    width: 1280

    property alias model: jobslistView.model
    property RDT rdt
    property TextViewer textViewer

    GridLayout{
        anchors.fill: parent
        rows: 2
        columns: 2

        Text {
            text: qsTr("Jobs Listing")
            font.pointSize: 10
        }

        Text {
            text: qsTr("Job Details")
            font.pointSize: 10

        }

        TableView{
            id: jobslistView

            Layout.fillHeight: true
            Layout.fillWidth: true


            onRowCountChanged:
            {
                if(rowCount > 0)
                    rdt.getJobDetails(0)
            }

            onClicked:
            {
                rdt.getJobDetails(currentRow)
            }

            TableViewColumn
            {
                id: nameCol
                title: "Name"
                role: "Name"
                movable: false
                width: 180

            }

            TableViewColumn
            {
                title: "Status"                
                role: "Status"
                movable: false
                width: 60
            }

            TableViewColumn
            {
                title: "Date"
                movable: false
                width: 80
                delegate: Text {
                    text: model.Created.substring(0, 10)
                    horizontalAlignment : Text.AlignHCenter

                }
            }

            TableViewColumn
            {
                title: "Id"
                role: "Id"
                movable: false
                width: 250
            }
        }


        TreeView
        {
            model: rdt.jobDetails
            Layout.fillHeight: true
            Layout.fillWidth: true


            onModelChanged: {
                //TODO:: expand inputs and outputs
                console.log("model changed")
                expand(model.index(10,0))
                expand(model.index(11,0))
            }

            TableViewColumn
            {
                title: "Name"
                role: "Name"
                width: 120

            }

            TableViewColumn
            {
                title: "Value"
                role: "Value"
                width: 480
                delegate: RowLayout{
                    Text{
                        id: valueText
                        text: {
                            if (model && model.Value && typeof model.Value === "string")
                                return model.Value
                            else
                                return ""
                        }
                    }


                    Button{
                        text: {
                            if(valueText.text.endsWith(".csv"))
                                return "Load"
                            else
                                return "Show"
                        }
                        visible: valueText.text.endsWith(".csv") ||
                                 valueText.text.endsWith(".out") ||
                                 valueText.text.endsWith(".err") ||
                                 valueText.text.endsWith(".log") ||
                                 valueText.text.includes("WorkflowTasks")
                        Layout.fillHeight: true
                        Layout.alignment: Qt.AlignRight

                        onClicked: {
                            rdt.loadResultFile(valueText.text)
                            if(!valueText.text.endsWith(".csv"))
                                textViewer.open()
                        }
                    }
                }
            }
        }
    }
}
