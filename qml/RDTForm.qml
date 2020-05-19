
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

import QtQuick 2.6
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.3
import QtGraphicalEffects 1.0
import Esri.RDT 1.0

Item {

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: view
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true
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

    Rectangle{
        anchors{
            top: parent.top
            right: parent.right
            rightMargin: 10
            topMargin: 10
        }

        border
        {
            color: "darkblue"
            width: 2
        }
        color: "#00000000"
        width: 50
        height: 50
        radius: 10

        MouseArea
        {
            onClicked: console.log("click")
        }

        Image {
            id: lockIcon
            source: "qrc:///Resources/Locked.png"
            scale: 0.1
        }
    }

    MenuBar {
        opacity: 0.75
        background: Rectangle {color: "light blue"}
            Menu {

                title: "&File"
                MenuItem { text: "&Open" }
                MenuItem { text: "&Save" }
                MenuItem { text: "Save &as" }
                MenuItem {
                    text: "E&xit"
                    onClicked: Qt.quit()
                }

                background: Rectangle {
                    implicitWidth: 200
                    color: "light blue"
                    opacity: 0.5
                    border.color: "dark blue"
                }
            }

            Menu {
                title: "&Edit"
                MenuItem { text: "&Copy" }
                MenuItem { text: "&Paste" }
                background: Rectangle {
                    implicitWidth: 200
                    color: "light blue"
                    opacity: 0.5
                    border.color: "dark blue"
                }

                MenuSeparator{
                    contentItem: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 2
                        color: "dark blue"
                        opacity: 0.25
                    }
                }

                MenuItem {
                    text: "&Refresh"
                    onClicked: model.refresh()
                }

            }

            Menu {
                title: "&Layers"

                Menu {
                    title: "&Add"
                    MenuItem {
                        text: "CS&V"
                        onClicked: fileDialog.open()
                    }
                    background: Rectangle {
                        implicitWidth: 200
                        color: "light blue"
                        opacity: 0.5
                        border.color: "dark blue"
                    }
                }

                background: Rectangle {
                    implicitWidth: 200
                    color: "light blue"
                    opacity: 0.5
                    border.color: "dark blue"
                }

            }

            Menu {
                title: "&Analysis"
                MenuItem {
                    text: "&Login"
                    onClicked: loginForm.open()
                }

                MenuItem {
                    text: "&Jobs"
                    onClicked: {
                        model.refreshJobs()
                        jobsForm.open()
                        jobsForm.model = model.jobsList
                    }
                }


                background: Rectangle {
                    implicitWidth: 200
                    color: "light blue"
                    opacity: 0.5
                    border.color: "dark blue"
                }

            }

            Menu {
                title: "&Help"
                MenuItem {text: "A&bout"}
                background: Rectangle {
                    implicitWidth: 200
                    color: "light blue"
                    opacity: 0.5
                    border.color: "dark blue"
                }
            }


    // Declare the C++ instance which creates the map etc. and supply the view

}

    RDT {
        id: model
        mapView: view
    }

    LoginForm{
        id: loginForm
        onAccepted: model.login(loginForm.username, loginForm.password)
    }

    JobsForm{
        id: jobsForm
        rdt: model
    }

    // dim window if map is drawing
    Rectangle {
        anchors.fill: parent
        color: "transparent"
        visible: model.mapDrawing

        RadialGradient {
            anchors.fill: parent
            opacity: 0.15
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.7; color: "black" }
            }
        }

        // pop up to show if MapView is drawing
        Rectangle {
            anchors.centerIn: parent
            width: 100
            height: 100
            radius: 3
            opacity: 0.85
            color: "#E0E0E0"
            border.color: "black"

            Column {
                anchors {
                    centerIn: parent
                    topMargin: 5
                }
                spacing: 5

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                    height: 60
                    running: true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font {
                        weight: Font.Black
                        pixelSize: 12
                    }
                    height: 20
                    horizontalAlignment: Text.AlignHCenter
                    text: "Loading..."
                }
            }
        }
    }


}
