import QtQuick

Item {
    id: root

    property int level: 0 // 0: OFF, 1: LOW, 2: MED, 3: HIGH
    property string label: "SEAT"

    signal clicked()

    implicitWidth: 84
    implicitHeight: 80

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: 12
        color: root.level > 0 ? "#2B1A1A" : "#1A1D24"
        border.color: root.level > 0 ? "#FF5A36" : "#2E3440"
        border.width: root.level > 0 ? 2 : 1

        Behavior on color { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }

        Column {
            anchors.centerIn: parent
            spacing: 8

            // 3 indicator bars for seat heater stage
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 4

                Repeater {
                    model: 3
                    Rectangle {
                        width: 14
                        height: 5
                        radius: 2
                        color: (index < root.level) ? "#FF5A36" : "#3B4252"
                        Behavior on color { ColorAnimation { duration: 150 } }
                    }
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.label
                color: root.level > 0 ? "#FFFFFF" : "#A0A8BA"
                font.pixelSize: 13
                font.bold: true
                font.family: "Helvetica Neue, Arial, sans-serif"
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.level === 0 ? "OFF" : (root.level === 1 ? "LOW" : (root.level === 2 ? "MED" : "HI"))
                color: root.level > 0 ? "#FF7A59" : "#606878"
                font.pixelSize: 10
                font.bold: true
                font.family: "Helvetica Neue, Arial, sans-serif"
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
        onPressed: bg.scale = 0.96
        onReleased: bg.scale = 1.0
        onCanceled: bg.scale = 1.0
    }
}
