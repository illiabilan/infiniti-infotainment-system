import QtQuick
import QtQuick.Controls

Item {
    id: root

    property string text: ""
    property string subText: ""
    property bool active: false
    property color activeColor: "#00B4D8"
    property color inactiveColor: "#1A1D24"
    property color textColor: "#FFFFFF"

    signal clicked()

    implicitWidth: 100
    implicitHeight: 80

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: 12
        color: root.active ? Qt.rgba(0.0, 0.7, 0.85, 0.18) : root.inactiveColor
        border.color: root.active ? root.activeColor : "#2E3440"
        border.width: root.active ? 2 : 1

        Behavior on color { ColorAnimation { duration: 150 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }

        // Top indicator light for active state
        Rectangle {
            anchors.top: parent.top
            anchors.topMargin: 6
            anchors.horizontalCenter: parent.horizontalCenter
            width: 24
            height: 4
            radius: 2
            color: root.active ? root.activeColor : "#3B4252"

            Behavior on color { ColorAnimation { duration: 150 } }
        }

        Column {
            anchors.centerIn: parent
            anchors.verticalCenterOffset: 4
            spacing: 3

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.text
                color: root.active ? "#FFFFFF" : "#D8DEE9"
                font.pixelSize: 18
                font.bold: true
                font.family: "Helvetica Neue, Arial, sans-serif"
            }

            Text {
                visible: root.subText.length > 0
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.subText
                color: root.active ? root.activeColor : "#8892B0"
                font.pixelSize: 11
                font.capitalization: Font.AllUppercase
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
