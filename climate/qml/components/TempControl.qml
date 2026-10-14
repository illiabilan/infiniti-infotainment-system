import QtQuick
import QtQuick.Controls

Item {
    id: root

    property string title: "ZONE"
    property real temperature: 21.5
    property color accentColor: "#00B4D8"

    signal tempStep(real delta)

    implicitWidth: 320
    implicitHeight: 360

    Rectangle {
        anchors.fill: parent
        radius: 20
        color: "#14171E"
        border.color: "#252B36"
        border.width: 1

        // Zone title
        Text {
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.title
            color: "#8892B0"
            font.pixelSize: 14
            font.bold: true
            font.letterSpacing: 2
            font.capitalization: Font.AllUppercase
            font.family: "Helvetica Neue, Arial, sans-serif"
        }

        // Stepper UP button
        Rectangle {
            id: btnUp
            anchors.top: parent.top
            anchors.topMargin: 56
            anchors.horizontalCenter: parent.horizontalCenter
            width: 120
            height: 48
            radius: 12
            color: "#1E232E"
            border.color: "#2F3746"
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "▲"
                color: "#E2E8F0"
                font.pixelSize: 18
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.tempStep(0.5)
                onPressed: btnUp.scale = 0.94
                onReleased: btnUp.scale = 1.0
                onCanceled: btnUp.scale = 1.0
            }
        }

        // Central temperature readout
        Item {
            anchors.centerIn: parent
            width: 220
            height: 100

            Row {
                anchors.centerIn: parent
                spacing: 2

                Text {
                    text: root.temperature.toFixed(1)
                    color: "#FFFFFF"
                    font.pixelSize: 56
                    font.bold: true
                    font.family: "Rajdhani, Helvetica Neue, Arial, sans-serif"
                }

                Text {
                    text: "°C"
                    color: root.accentColor
                    font.pixelSize: 26
                    font.bold: true
                    anchors.top: parent.top
                    anchors.topMargin: 8
                    font.family: "Rajdhani, Helvetica Neue, Arial, sans-serif"
                }
            }
        }

        // Stepper DOWN button
        Rectangle {
            id: btnDown
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 24
            anchors.horizontalCenter: parent.horizontalCenter
            width: 120
            height: 48
            radius: 12
            color: "#1E232E"
            border.color: "#2F3746"
            border.width: 1

            Text {
                anchors.centerIn: parent
                text: "▼"
                color: "#E2E8F0"
                font.pixelSize: 18
            }

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: root.tempStep(-0.5)
                onPressed: btnDown.scale = 0.94
                onReleased: btnDown.scale = 1.0
                onCanceled: btnDown.scale = 1.0
            }
        }
    }
}
