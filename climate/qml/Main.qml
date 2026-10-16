import QtQuick
import QtQuick.Window
import App.Theme

Window {
    id: window
    width: 1280
    height: 160
    minimumWidth: 1280
    minimumHeight: 160
    maximumWidth: 1280
    maximumHeight: 160
    visible: true
    title: "Infiniti G35 — Climate Ribbon"
    color: Theme.colors.surface

    ClimateRibbonView {
        anchors.fill: parent
    }
}
