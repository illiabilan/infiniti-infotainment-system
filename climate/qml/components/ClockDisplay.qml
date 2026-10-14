import QtQuick

Item {
    id: root

    property string timeString: "12:00"
    property string dateString: "MON, JAN 1"
    property int outsideTemp: 20
    property bool dualMode: false

    implicitWidth: 320
    implicitHeight: 180

    Rectangle {
        anchors.fill: parent
        radius: 20
        color: "#12151C"
        border.color: "#252B36"
        border.width: 1

        Column {
            anchors.centerIn: parent
            spacing: 4

            // Branding / Model
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "INFINITI G35"
                color: "#6C7A9C"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 3
                font.family: "Helvetica Neue, Arial, sans-serif"
            }

            // Big Clock
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.timeString
                color: "#FFFFFF"
                font.pixelSize: 52
                font.bold: true
                font.family: "Rajdhani, Helvetica Neue, Arial, sans-serif"
            }

            // Date & Exterior Temp
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 14

                Text {
                    text: root.dateString
                    color: "#8892B0"
                    font.pixelSize: 12
                    font.bold: true
                    font.letterSpacing: 1
                    font.family: "Helvetica Neue, Arial, sans-serif"
                }

                Rectangle {
                    width: 1
                    height: 12
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#2E3440"
                }

                Text {
                    text: "OUTSIDE " + root.outsideTemp + "°C"
                    color: "#00B4D8"
                    font.pixelSize: 12
                    font.bold: true
                    font.letterSpacing: 1
                    font.family: "Helvetica Neue, Arial, sans-serif"
                }
            }
        }
    }
}
