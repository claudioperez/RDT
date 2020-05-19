import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

Dialog {
    property alias username: username.text
    property alias password: password.text

    id: loginDialog
    title: "Login to DesignSafe-CI"
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal:  true
    anchors.centerIn: parent

    GridLayout {
        id: grid
        columns: 2
        anchors.fill: parent
        anchors.margins: 10
        rowSpacing: 10
        columnSpacing: 10

        Label {
            text: "Username:"
        }

        TextField {
            id: username
            text: ""
            Layout.fillWidth: true
        }

        Label {
            text: "Password";
        }

        TextField {
            id: password
            text: ""
            passwordCharacter: '*'
            Layout.fillWidth: true
            echoMode: TextInput.Password
        }

    }

}
