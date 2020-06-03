
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
            }
            Menu {
                title: "&Analysis"
                MenuItem {
                    text: "&Login"
                    onTriggered: loginForm.open()
                }

                Menu {
                    title: "&Jobs"

                    MenuItem{
                        text: "&Manage"
                        onTriggered: {
                            rdt.model.refreshJobs()
                            jobsForm.open()
                            jobsForm.model = rdt.model.jobsList
                        }
                    }

                    MenuItem{
                        text: "&Submit"
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
    }

    SubmitJobForm{
        id: submitJobForm
        rdt: rdt.model
    }

    FileDialog {
        id: fileDialog
        folder: "file:///C:/SandBox/Anchorage/GIS/"
        title: "Please select a layer to add to map"
        nameFilters: [ "CSV files (*.csv)", "All files (*)" ]
        onAccepted: {
            model.addCSVLayer(fileDialog.fileUrl)
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
        }

    }
}
