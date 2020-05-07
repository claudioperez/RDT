import QtQuick 2.12
import TableModel 0.1

TableView {
    anchors.fill: parent
    columnSpacing: 1
    rowSpacing: 1
    clip: true

    model: TableModel {
    }

    delegate: Rectangle {
        implicitWidth: 200
        implicitHeight: 50

        Text {
            text: display
        }
    }
}
