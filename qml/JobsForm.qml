import QtQuick 2.12
import QtQuick.Controls 1.4 as QC1
import QtQuick.Controls 2.12 as QC2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import Esri.RDT 1.0

QC2.Dialog {
    standardButtons: QC2.DialogButtonBox.Ok | QC2.DialogButtonBox.Cancel
    anchors.centerIn: parent
    height: 480
    width: 1280

    property alias model: jobslistView.model
    property RDT rdt

    RowLayout{
        anchors.fill: parent

        QC1.TableView{
            id: jobslistView
            Layout.fillHeight: true
            Layout.maximumHeight: 480
            Layout.preferredHeight: 400
            Layout.preferredWidth: 800
            Layout.maximumWidth: 1280

            //anchors.fill: parent
    //        columnSpacing: 2
    //        rowSpacing: 2
            clip: true
            width: parent.width
            height: parent.height
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            onDoubleClicked:
            {
                var jobDetails = rdt.getJob(currentRow)
                var jobJson = JSON.parse(jobDetails)
                nameLabel.text = jobJson["name"]
                idLabel.text = jobJson["id"]
                startedLabel.text = jobJson["remoteStarted"]
                appIdLabel.text = jobJson["appId"]
                endedLabel.text = jobJson["ended"]
                submittedLabel.text = jobJson["created"]
                outputLabel.text = jobJson["archivePath"]

            }

            QC1.TableViewColumn
            {
                title: "Name"
                role: "Name"
                movable: false
                width: 180
            }

            QC1.TableViewColumn
            {
                title: "Status"
                role: "Status"
                movable: false
                width: 60
            }

            QC1.TableViewColumn
            {
                title: "Id"
                role: "Id"
                movable: false
                width: 250
            }


            QC1.TableViewColumn
            {
                title: "Created"
                role: "Created"
                movable: false
                width: 100

            }

            QC1.TableViewColumn
            {
                title: "Start Time"
                role: "Start"
                movable: false
                width: 100

            }

            QC1.TableViewColumn
            {
                title: "End Time"
                role: "End"
                movable: false
                width: 100


            }



//            Layout.minimumWidth: 200
//            Layout.minimumHeight: 100
//            Layout.preferredWidth: 600
//            Layout.preferredHeight: 480




    //        delegate: Rectangle {
    //            implicitWidth: 80
    //            implicitHeight: 30
    //            border.color: "grey"
    //            border.width: 1
    //            Text {
    //                text: display
    //                font.pointSize: 12
    //                anchors.centerIn: parent
    //                elide: Text.ElideRight
    //                font.preferShaping: false

    //            }
    //            color: "#EEE"
    //        }



        }

        QC2.GroupBox {
            label: Text {
                text: "Job Details"
                anchors.horizontalCenter: parent.horizontalCenter
                font.pointSize: 12
                font.bold: true

            }
            id: jobDetails
            Layout.fillWidth: true
            Layout.fillHeight: true
            //title: "Job Details"

            GridLayout{
                columns: 2

                QC2.Label{
                    text: "Id:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id: idLabel}


                QC2.Label{
                    text: "Name:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id: nameLabel}

                QC2.Label{
                    text: "App Id:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id: appIdLabel}

                QC2.Label{
                    text: "Submitted:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id:submittedLabel}

                QC2.Label{
                    text: "Started:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id:startedLabel}

                QC2.Label{
                    text: "Ended:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id:endedLabel}

                QC2.Label{
                    text: "Output:"
                    font.bold: true
                    font.pointSize: 10
                }

                QC2.Label{id:outputLabel}


                QC2.Button
                {
                    text: "Load Results"
                    Layout.columnSpan: 2
                    Layout.alignment: Qt.AlignCenter
                    onClicked: rdt.loadResults(outputLabel.text)

                }

            }

        }



    }
}
