import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import SimCenter.RDT 1.0

Dialog {
    property RDT rdt

    //anchors.centerIn: parent
    title: "Job Definition"
    standardButtons: Dialog.Ok  | Dialog.Cancel

    onAccepted: {
        var job = new Object();
        job.name = jobname.text
        job.appId = appId.currentText
        var nodesCount = Number(nodes.text)
        job.nodeCount = nodesCount
        job.processorsPerNode = nodesCount * 64
        var time =  hours.text + ':' + minutes.text + ':' + seconds.text
        job.maxRunTime = time
        job.archive = true
        job.archiveSystem = "designsafe.storage.default"
        job.parameters = new Object()
        job.parameters.buildingsCount = buildingsCount.text
        job.parameters.Logging = true

        job.inputs = new Object()
        job.inputs.configFile = configText.text
        job.inputs.dataFiles = new Array()

        for(var child in inputsList.contentItem.children) {
            if(inputsList.contentItem.children[child].objectName === "text")
                job.inputs.dataFiles.push(inputsList.contentItem.children[child].text)
        }

        rdt.submitJob(JSON.stringify(job))
    }



    GridLayout
    {
        columns: 3

        GroupBox{
            title: "General"

            GridLayout{
                Layout.fillWidth: true
                columns: 2

                Text{
                    text: "Name:"
                }

                TextField {
                    id: jobname
                    text: "RDT-Sample-Job"
                    selectByMouse: true
                    Layout.fillWidth: true
                }

                Text{
                    text: "Version:"
                }

                ComboBox {
                    id: appId
                    model: [ "rWHALE-1.1.0u3", "rWHALE-2.0-pre" ]
                    Layout.minimumWidth: 150
                }

            }
        }

        GroupBox{
            title: "Compute Resources"

            GridLayout{
                Layout.fillWidth: true
                columns: 2

                Text{
                    text: "Nodes:"
                }

                TextField {
                    id: nodes
                    text: "1"
                    selectByMouse: true
                    validator: IntValidator {bottom: 1; top: 128;}
                    Layout.fillWidth: true
                    Layout.preferredWidth: 60
                }

                Text{
                    text: "Max. Time:"
                }

                RowLayout{
                    Layout.fillWidth: true

                    TextField {
                        id: hours
                        text: "00"
                        selectByMouse: true
                        validator: IntValidator {bottom: 0; top: 128}
                        Layout.preferredWidth: 30
                    }

                    TextField {
                        id: minutes
                        text: "30"
                        selectByMouse: true
                        validator: IntValidator {bottom: 0; top: 60}
                        Layout.preferredWidth: 30
                    }

                    TextField {
                        id: seconds
                        text: "00"
                        selectByMouse: true
                        validator: IntValidator {bottom: 0; top: 60}
                        Layout.preferredWidth: 30
                    }
                }

            }
        }

        GroupBox{
            Layout.fillHeight: true
            title: "Parameters"
            Layout.preferredWidth: 30

            GridLayout{
                Layout.fillWidth: true
                columns: 2

                Text{
                    text: "Buildings:"
                }

                TextField {
                    id: buildingsCount
                    text: "10"
                    selectByMouse: true
                    validator: IntValidator {bottom: 1}
                    Layout.preferredWidth: 80
                }
            }
        }

        GroupBox{
            Layout.fillWidth: true
            Layout.columnSpan: 3
            title: "Inputs"

            GridLayout
            {
                anchors.fill: parent
                columns: 2

                Text{
                    text: "Config:"
                }

                TextField {
                    id: configText
                    selectByMouse: true
                    Layout.fillWidth: true
                    text: "agave://designsafe.storage.community/SimCenter/Datasets/AnchorageM7/AnchorageM7.0.json"

                }

                Text {
                    text: qsTr("Data")
                }

                ListView
                {
                    id: inputsList
                    Layout.fillWidth: true
                    Layout.preferredHeight: 55

                    model: rdt.inputs
                    spacing: 30
                    delegate: TextField {
                        objectName: "text"
                            width: configText.width
                            text: modelData
                    }

                }

            }



        }
    }
}
