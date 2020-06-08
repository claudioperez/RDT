import QtQuick 2.12
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import SimCenter.RDT 1.0

Dialog {
    title: "Text File Viewer"
    standardButtons: Dialog.Ok
    height: 720
    width: 480
    modality: "WindowModal"

    property RDT rdt

    TextArea
    {
        text: rdt.textFileContents
        anchors.fill: parent
        readOnly: true

    }
}
