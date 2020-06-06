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

    RowLayout{
        anchors.fill: parent

        TableView{
            id: jobslistView

            Layout.fillHeight: true
            Layout.fillWidth: true

            onRowCountChanged:
            {
                if(rowCount > 0)
                    rdt.getJob(0)
            }

            onClicked:
            {
                rdt.getJob(currentRow)
            }

            TableViewColumn
            {
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
                width: 300
            }
        }
    }
}
