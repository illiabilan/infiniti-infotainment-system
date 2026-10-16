import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: emulatorWindow
    visible: true
    width: 1080
    height: 560
    minimumWidth: 720
    minimumHeight: 360
    title: "Infiniti G35 — ECU Telemetry Testbench"
    color: "#080F18"

    EcuSimulatorPanel {
        id: ecuSimulator
        anchors.fill: parent

        onBtnUpPressed: if (typeof controller !== "undefined" && controller) controller.prevTripPage()
        onBtnDownPressed: if (typeof controller !== "undefined" && controller) controller.nextTripPage()
        onBtnOkPressed: if (typeof controller !== "undefined" && controller) controller.resetActiveTripPage()
        onBtnInfoPressed: if (typeof controller !== "undefined" && controller) controller.triggerInfoMenu()
        onBtnBackPressed: if (typeof controller !== "undefined" && controller) controller.triggerInfoMenu()
    }
}
