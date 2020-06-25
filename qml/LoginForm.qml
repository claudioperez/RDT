import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0

Dialog {
    property alias username: username.text
    property alias password: password.text

    id: loginDialog
    title: "Login to DesignSafe-CI"
    standardButtons: Dialog.Ok | Dialog.Cancel
    //modal:  true
    //anchors.centerIn: parent

    GridLayout {
        id: grid
        columns: 2
        anchors.fill: parent


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
            Layout.fillWidth: true
            echoMode: TextInput.Password
        }

    }

}
