
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

import QtQuick.Controls 1.4
import SimCenter.RDT 1.0
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

ApplicationWindow {
    visible: true
    width: 1440
    height: 900

    menuBar:MenuBar {
            Menu {

                title: "&File"
                MenuItem { text: "&Open" }
                MenuItem { text: "&Save" }
                MenuItem { text: "Save &as" }
                MenuItem {
                    text: "E&xit"
                    onTriggered: Qt.quit()
                }
            }

            Menu {
                title: "&Edit"
                MenuItem { text: "&Copy" }
                MenuItem { text: "&Paste" }
                MenuSeparator{
                }

                MenuItem {
                    text: "&Refresh"
                    onTriggered: rdt.model.refresh()
                }

            }

            Menu {
                title: "&Layers"

                Menu {
                    title: "&Add"
                    MenuItem {
                        text: "CS&V"
                        onTriggered: fileDialog.open()
                    }
                }
                MenuItem{
                    text: "&Manage"
                    onTriggered: {
                        layersForm.open()
                        layersForm.model = rdt.model.layers
                        layersForm.rendererModel = rdt.model.renderer
                    }
                }
            }
            Menu {
                title: "&Analysis"
                MenuItem {
                    text: "&Login"
                    enabled: !rdt.model.loggendIn
                    onTriggered: loginForm.open()
                }

                Menu {
                    title: "&Jobs"
                    enabled: rdt.model.loggendIn

                    MenuItem{
                        text: "&Manage"
                        enabled: rdt.model.loggendIn
                        onTriggered: {
                            rdt.model.refreshJobs()
                            jobsForm.open()
                            jobsForm.model = rdt.model.jobsList
                        }
                    }

                    MenuItem{
                        text: "&Submit"
                        enabled: rdt.model.loggendIn
                        onTriggered: {
                            submitJobForm.open()
                        }
                    }
                }

            }

            Menu {
                title: "&Help"
                MenuItem {text: "A&bout"}
            }


    // Declare the C++ instance which creates the map etc. and supply the view

}

    LoginForm{
        id: loginForm
        onAccepted: rdt.model.login(loginForm.username, loginForm.password)
    }

    JobsForm{
        id: jobsForm
        rdt: rdt.model
        textViewer: textViewer
    }

    SubmitJobForm{
        id: submitJobForm
        rdt: rdt.model
    }

    LayersTable{
        id: layersForm
        rdt: rdt.model
    }

    TextViewer{
        id: textViewer
        rdt: rdt.model
    }

    FileDialog {
        id: fileDialog
        folder: "file:///C:/SandBox/Anchorage/GIS/"
        title: "Please select a layer to add to map"
        nameFilters: [ "CSV files (*.csv)", "All files (*)" ]
        onAccepted: {
            rdt.model.addCSVLayer(fileDialog.fileUrl)
        }

    }

    ColumnLayout{
        anchors.fill: parent
        RDTForm {
            id: rdt
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        StatusBar{
            id: statusBar
            Layout.fillWidth: true

            RowLayout{
                Label {
                    text: {
                        if(rdt.model.loggendIn)
                            return "Logged In"
                        else
                            return "Logged Out"
                    }
                }
            }

        }

    }
}
