import QtQuick

Item {
    id: root

    property int airflowMode: 0 // 0: Face, 1: Face+Feet, 2: Feet, 3: Defrost+Feet
    property color activeColor: "#00B4D8"

    signal modeSelected(int mode)

    implicitWidth: 380
    implicitHeight: 80

    readonly property var modes: [
        { name: "FACE", symbol: "⇥ FACE" },
        { name: "BI-LEVEL", symbol: "⇄ SPLIT" },
        { name: "FEET", symbol: "⇣ FLOOR" },
        { name: "DEF/FEET", symbol: "⇡ DEF/FLR" }
    ]

    Row {
        anchors.fill: parent
        spacing: 8

        Repeater {
            model: root.modes

            Rectangle {
                id: modeBtn
                width: (root.width - (root.modes.length - 1) * 8) / root.modes.length
                height: root.height
                radius: 12
                color: root.airflowMode === index ? Qt.rgba(0.0, 0.7, 0.85, 0.18) : "#1A1D24"
                border.color: root.airflowMode === index ? root.activeColor : "#2E3440"
                border.width: root.airflowMode === index ? 2 : 1

                Behavior on color { ColorAnimation { duration: 150 } }
                Behavior on border.color { ColorAnimation { duration: 150 } }

                Column {
                    anchors.centerIn: parent
                    spacing: 4

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.symbol
                        color: root.airflowMode === index ? root.activeColor : "#A0A8BA"
                        font.pixelSize: 13
                        font.bold: true
                        font.family: "Helvetica Neue, Arial, sans-serif"
                    }

                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.name
                        color: root.airflowMode === index ? "#FFFFFF" : "#606878"
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 1
                        font.family: "Helvetica Neue, Arial, sans-serif"
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.modeSelected(index)
                    onPressed: modeBtn.scale = 0.95
                    onReleased: modeBtn.scale = 1.0
                    onCanceled: modeBtn.scale = 1.0
                }
            }
        }
    }
}
