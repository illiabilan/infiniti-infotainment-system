import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import QtQuick.Layouts
import App.Theme
import App.Components
import App.Icons

Item {
    id: root

    width: 1280
    height: 160

    readonly property var vs: vehicleState
    readonly property var clock: climateController

    // --- Compact chips component ---
    component Chip: Surface {
        id: chip
        property string label
        property bool active: false
        property color activeColor: Theme.colors.accent

        implicitWidth: label.length * 9 + 16
        implicitHeight: 22
        radius: 4
        color: active ? activeColor : Theme.colors.tile
        neomorph: true
        pressed: active

        Text {
            anchors.centerIn: parent
            text: label
            color: active ? Theme.colors.onaccent : Theme.colors.textSecondary
            font {
                family: Theme.typography.family
                pixelSize: 8
                bold: true
                weight: Theme.typography.weightMedium
            }
        }
    }

    // --- Fan bar ---
    component FanBars: Item {
        id: bars
        property int level: 3

        width: 9 * 8
        height: 14

        Repeater {
            model: 8
            Rectangle {
                width: 9
                height: 14
                x: index * 11
                y: 0
                radius: 2
                color: index < bars.level ? Theme.colors.accent : Theme.colors.sliderTrack
            }
        }
    }

    // --- Seat heat indicator ---
    component SeatHeatLev: Item {
        id: heat
        property int level: 0

        implicitWidth: 34
        implicitHeight: 7

        RowLayout {
            anchors.fill: parent
            spacing: 2

            Repeater {
                model: 3
                Rectangle {
                    width: 10
                    height: 5
                    radius: 1
                    color: index < heat.level ? Theme.colors.accent : Theme.colors.iconMuted
                    opacity: index < heat.level ? 1.0 : 0.5
                }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        // ====== ZONE 1: Driver ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: true

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 8

                // Seat heat
                ColumnLayout {
                    Layout.preferredWidth: 30
                    Layout.fillHeight: true
                    spacing: 1

                    AppIcon {
                        Layout.alignment: Qt.AlignHCenter
                        source: Icons.seat
                        size: 10
                        color: Theme.colors.icon
                        Layout.preferredHeight: 16
                    }

                    SeatHeatLev { level: vs ? vs.driverSeatHeat : 0; Layout.alignment: Qt.AlignHCenter }

                    Text {
                        text: {
                            switch(vs ? vs.driverSeatHeat : 0) {
                            case 1: return "LO";
                            case 2: return "MED";
                            case 3: return "HI";
                            default: return "";
                            }
                        }
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 6
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.accent
                    }
                }

                // Divider
                Rectangle {
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                    color: Theme.colors.divider
                }

                // Driver temp
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 1

                    Text {
                        text: "DRV"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 7
                            bold: true
                            letterSpacing: 1
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.textSecondary
                    }

                    Text {
                        text: String((vs ? vs.driverTemp : 21.5).toFixed(1)) + "°C"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 28
                            bold: true
                            weight: Font.DemiBold
                        }
                        color: Theme.colors.textPrimary
                    }
                }
            }
        }

        // ====== ZONE 2: System Status ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Text {
                    text: "CLIMATE"
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 7
                        bold: true
                        letterSpacing: 1.5
                        weight: Theme.typography.weightBold
                    }
                    color: Theme.colors.textSecondary
                }

                // AC / AUTO / RECIRC chips
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 4

                    Chip { label: "A/C"; active: vs ? vs.acActive : true }
                    Chip { label: "AUTO"; active: vs ? vs.autoActive : true }
                    Chip { label: "RECIRC"; active: vs ? vs.recircActive : false }
                }

                // Airflow + Dual
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 4

                    Text {
                        text: {
                            switch(vs ? vs.airflowMode : 0) {
                            case 0: return "FACE";
                            case 1: return "FACE+FEET";
                            case 2: return "FEET";
                            case 3: return "DEF+FEET";
                            default: return "FACE";
                            }
                        }
                        font {
                            family: Theme.typography.family
                            pixelSize: 8
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.textPrimary
                    }

                    Rectangle {
                        width: 1
                        height: 14
                        color: Theme.colors.divider
                    }

                    Text {
                        text: vs && vs.dualMode ? "DUAL" : "SYNC"
                        font {
                            family: Theme.typography.family
                            pixelSize: 8
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.success
                    }
                }
            }
        }

        // ====== ZONE 3: Clock + Fan ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: true

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 2

                Text {
                    text: clock ? clock.currentTime : "12:00"
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 28
                        bold: true
                        weight: Font.DemiBold
                    }
                    color: Theme.colors.textPrimary
                }

                Text {
                    text: (clock ? clock.currentDate : "MON 01") + "  " + String(vs ? vs.outsideTemp : 20) + "°C"
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 8
                        bold: true
                        weight: Theme.typography.weightMedium
                    }
                    color: Theme.colors.textSecondary
                }

                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 6

                    Text {
                        text: "F" + (vs ? vs.fanSpeed : 3)
                        font {
                            family: Theme.typography.family
                            pixelSize: 9
                            bold: true
                            weight: Theme.typography.weightBold
                        }
                        color: Theme.colors.accent
                    }

                    FanBars { level: vs ? vs.fanSpeed : 3 }
                }
            }
        }

        // ====== ZONE 4: Defrost + Passenger ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: true

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                // Defrost status
                ColumnLayout {
                    Layout.preferredWidth: 60
                    Layout.fillHeight: true
                    spacing: 2

                    Text {
                        text: "DEF"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 7
                            bold: true
                            letterSpacing: 1
                            weight: Theme.typography.weightBold
                        }
                        color: Theme.colors.textSecondary
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignHCenter
                        spacing: 3

                        Chip {
                            label: "F"
                            active: vs ? vs.frontDefrostActive : false
                            activeColor: "#FF6B6B"
                        }
                        Chip {
                            label: "R"
                            active: vs ? vs.rearDefrostActive : false
                            activeColor: "#FFAA00"
                        }
                    }

                    Text {
                        text: {
                            const f = vs && vs.frontDefrostActive;
                            const r = vs && vs.rearDefrostActive;
                            if (f && r) return "BOTH";
                            if (f) return "F";
                            if (r) return "R";
                            return "";
                        }
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 7
                            bold: true
                            weight: Theme.typography.weightBold
                        }
                        color: ((vs && vs.frontDefrostActive) || (vs && vs.rearDefrostActive))
                               ? Theme.colors.textPrimary : Theme.colors.iconMuted
                    }
                }

                // Divider
                Rectangle {
                    width: 1
                    height: 60
                    color: Theme.colors.divider
                }

                // Passenger temp
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 1

                    Text {
                        text: "PWR"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 7
                            bold: true
                            letterSpacing: 1
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.textSecondary
                    }

                    Text {
                        text: String((vs ? vs.passengerTemp : 21.5).toFixed(1)) + "°C"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 28
                            bold: true
                            weight: Font.DemiBold
                        }
                        color: Theme.colors.textPrimary
                    }
                }

                // Passenger seat heat
                ColumnLayout {
                    Layout.preferredWidth: 30
                    Layout.fillHeight: true
                    spacing: 1

                    AppIcon {
                        Layout.alignment: Qt.AlignHCenter
                        source: Icons.seat
                        size: 10
                        color: Theme.colors.icon
                        Layout.preferredHeight: 16
                    }

                    SeatHeatLev { level: vs ? vs.passengerSeatHeat : 0; Layout.alignment: Qt.AlignHCenter }

                    Text {
                        text: {
                            switch(vs ? vs.passengerSeatHeat : 0) {
                            case 1: return "LO";
                            case 2: return "MED";
                            case 3: return "HI";
                            default: return "";
                            }
                        }
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 6
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.accent
                    }
                }
            }
        }
    }
}
