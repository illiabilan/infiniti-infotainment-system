import QtQuick

Item {
    id: root

    property int fanSpeed: 3 // 0 to 7
    property color activeColor: "#00B4D8"

    signal speedChanged(int newSpeed)
    signal speedStep(int delta)

    implicitWidth: 380
    implicitHeight: 120

    Rectangle {
        anchors.fill: parent
        radius: 16
        color: "#14171E"
        border.color: "#252B36"
        border.width: 1

        Row {
            anchors.centerIn: parent
            spacing: 16

            // Fan decrease button
            Rectangle {
                id: btnMinus
                width: 48
                height: 48
                radius: 10
                color: "#1E232E"
                border.color: "#2F3746"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "－"
                    color: "#FFFFFF"
                    font.pixelSize: 22
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.speedStep(-1)
                    onPressed: btnMinus.scale = 0.94
                    onReleased: btnMinus.scale = 1.0
                    onCanceled: btnMinus.scale = 1.0
                }
            }

            // Fan Speed Bars Container
            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8

                // Header
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 8

                    Text {
                        text: "FAN SPEED"
                        color: "#8892B0"
                        font.pixelSize: 12
                        font.bold: true
                        font.letterSpacing: 1
                        font.family: "Helvetica Neue, Arial, sans-serif"
                    }

                    Text {
                        text: root.fanSpeed === 0 ? "OFF" : root.fanSpeed.toString()
                        color: root.fanSpeed > 0 ? root.activeColor : "#5B6477"
                        font.pixelSize: 12
                        font.bold: true
                        font.family: "Helvetica Neue, Arial, sans-serif"
                    }
                }

                // Segmented 7 bars
                Row {
                    spacing: 6

                    Repeater {
                        model: 7

                        Rectangle {
                            id: bar
                            width: 26
                            height: 18 + index * 4
                            radius: 3
                            anchors.bottom: parent.bottom
                            color: (index < root.fanSpeed) ? root.activeColor : "#252B36"

                            Behavior on color { ColorAnimation { duration: 150 } }

                            MouseArea {
                                anchors.fill: parent
                                anchors.margins: -4
                                cursorShape: Qt.PointingHandCursor
                                onClicked: root.speedChanged(index + 1)
                            }
                        }
                    }
                }
            }

            // Fan increase button
            Rectangle {
                id: btnPlus
                width: 48
                height: 48
                radius: 10
                color: "#1E232E"
                border.color: "#2F3746"
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: "＋"
                    color: "#FFFFFF"
                    font.pixelSize: 22
                    font.bold: true
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.speedStep(1)
                    onPressed: btnPlus.scale = 0.94
                    onReleased: btnPlus.scale = 1.0
                    onCanceled: btnPlus.scale = 1.0
                }
            }
        }
    }
}
