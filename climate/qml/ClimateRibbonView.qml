import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "components"

Item {
    id: root

    width: 1920
    height: 480

    // Reference to backend controller (defaulting to context property if registered)
    property var controller: typeof climateController !== "undefined" ? climateController : null

    // Background gradient and automotive border styling
    Rectangle {
        anchors.fill: parent
        color: "#0B0D12"

        // Subtle gradient sheen
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#11141D" }
            GradientStop { position: 0.5; color: "#0B0D12" }
            GradientStop { position: 1.0; color: "#07080B" }
        }

        // Top edge accent line
        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 2
            color: "#1E232E"
        }

        // Bottom edge accent line
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 2
            color: "#1E232E"
        }
    }

    // Main 5-zone horizontal layout
    RowLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 24

        // ==========================================
        // 1. DRIVER ZONE (LEFT)
        // ==========================================
        RowLayout {
            Layout.preferredWidth: 380
            Layout.fillHeight: true
            spacing: 12

            SeatHeaterButton {
                Layout.preferredWidth: 76
                Layout.preferredHeight: 120
                Layout.alignment: Qt.AlignVCenter
                label: "DRIVER"
                level: root.controller ? root.controller.driverSeatHeat : 1
                onClicked: {
                    if (root.controller) root.controller.cycleDriverSeatHeat();
                }
            }

            TempControl {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "DRIVER ZONE"
                accentColor: "#00B4D8"
                temperature: root.controller ? root.controller.driverTemp : 21.5
                onTempStep: (delta) => {
                    if (root.controller) root.controller.stepDriverTemp(delta);
                }
            }
        }

        // ==========================================
        // 2. CENTER-LEFT: PRIMARY TOGGLES & AIRFLOW
        // ==========================================
        ColumnLayout {
            Layout.preferredWidth: 360
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 20

            Text {
                text: "SYSTEM MODES"
                color: "#6C7A9C"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 2
                font.family: "Helvetica Neue, Arial, sans-serif"
            }

            // AC, AUTO, RECIRC
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                ClimateButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    text: "A/C"
                    subText: "COMPRESSOR"
                    active: root.controller ? root.controller.acActive : true
                    activeColor: "#00B4D8"
                    onClicked: {
                        if (root.controller) root.controller.toggleAc();
                    }
                }

                ClimateButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    text: "AUTO"
                    subText: "CLIMATE"
                    active: root.controller ? root.controller.autoActive : true
                    activeColor: "#48CAE4"
                    onClicked: {
                        if (root.controller) root.controller.toggleAuto();
                    }
                }

                ClimateButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    text: "RECIRC"
                    subText: "CABIN AIR"
                    active: root.controller ? root.controller.recircActive : false
                    activeColor: "#FFAA00"
                    onClicked: {
                        if (root.controller) root.controller.toggleRecirc();
                    }
                }
            }

            // Airflow distribution buttons
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: "AIRFLOW DISTRIBUTION"
                    color: "#6C7A9C"
                    font.pixelSize: 11
                    font.bold: true
                    font.letterSpacing: 2
                    font.family: "Helvetica Neue, Arial, sans-serif"
                }

                AirflowControl {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    airflowMode: root.controller ? root.controller.airflowMode : 0
                    onModeSelected: (mode) => {
                        if (root.controller) root.controller.setAirflowMode(mode);
                    }
                }
            }
        }

        // ==========================================
        // 3. CENTER: LUXURY CLOCK & INFOTAINMENT STATUS
        // ==========================================
        ColumnLayout {
            Layout.preferredWidth: 320
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 16

            ClockDisplay {
                Layout.fillWidth: true
                Layout.preferredHeight: 230
                timeString: root.controller ? root.controller.currentTime : "16:20"
                dateString: root.controller ? root.controller.currentDate : "WED, OCT 14"
                outsideTemp: root.controller ? root.controller.outsideTemp : 19
                dualMode: root.controller ? root.controller.dualMode : false
            }

            // Dual / Sync quick toggle
            ClimateButton {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                text: "DUAL ZONE"
                subText: (root.controller && root.controller.dualMode) ? "INDEPENDENT" : "SYNCED"
                active: root.controller ? root.controller.dualMode : false
                activeColor: "#90E0EF"
                onClicked: {
                    if (root.controller) root.controller.toggleDual();
                }
            }
        }

        // ==========================================
        // 4. CENTER-RIGHT: DEFROST & FAN SPEED
        // ==========================================
        ColumnLayout {
            Layout.preferredWidth: 360
            Layout.fillHeight: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 20

            Text {
                text: "DEFROST CONTROLS"
                color: "#6C7A9C"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 2
                font.family: "Helvetica Neue, Arial, sans-serif"
            }

            // Front & Rear Defrost
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                ClimateButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    text: "FRONT"
                    subText: "MAX DEFROST"
                    active: root.controller ? root.controller.frontDefrostActive : false
                    activeColor: "#FF6B6B"
                    onClicked: {
                        if (root.controller) root.controller.toggleFrontDefrost();
                    }
                }

                ClimateButton {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 90
                    text: "REAR"
                    subText: "HEATED GLASS"
                    active: root.controller ? root.controller.rearDefrostActive : false
                    activeColor: "#FFAA00"
                    onClicked: {
                        if (root.controller) root.controller.toggleRearDefrost();
                    }
                }
            }

            // Fan Speed
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: "BLOWER VENTILATION"
                    color: "#6C7A9C"
                    font.pixelSize: 11
                    font.bold: true
                    font.letterSpacing: 2
                    font.family: "Helvetica Neue, Arial, sans-serif"
                }

                FanControl {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 110
                    fanSpeed: root.controller ? root.controller.fanSpeed : 3
                    onSpeedChanged: (speed) => {
                        if (root.controller) root.controller.setFanSpeed(speed);
                    }
                    onSpeedStep: (delta) => {
                        if (root.controller) root.controller.stepFanSpeed(delta);
                    }
                }
            }
        }

        // ==========================================
        // 5. PASSENGER ZONE (RIGHT)
        // ==========================================
        RowLayout {
            Layout.preferredWidth: 380
            Layout.fillHeight: true
            spacing: 12

            TempControl {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "PASSENGER ZONE"
                accentColor: "#00B4D8"
                temperature: root.controller ? root.controller.passengerTemp : 21.5
                onTempStep: (delta) => {
                    if (root.controller) root.controller.stepPassengerTemp(delta);
                }
            }

            SeatHeaterButton {
                Layout.preferredWidth: 76
                Layout.preferredHeight: 120
                Layout.alignment: Qt.AlignVCenter
                label: "PASS"
                level: root.controller ? root.controller.passengerSeatHeat : 0
                onClicked: {
                    if (root.controller) root.controller.cyclePassengerSeatHeat();
                }
            }
        }
    }
}
