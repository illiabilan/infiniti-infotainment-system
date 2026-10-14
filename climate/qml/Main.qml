import QtQuick
import QtQuick.Window

Window {
    id: window
    width: 1920
    height: 480
    minimumWidth: 960
    minimumHeight: 240
    visible: true
    title: "Infiniti G35 — Climate Ribbon (1920x480)"
    color: "#0B0D12"

    ClimateRibbonView {
        anchors.fill: parent
    }
}
