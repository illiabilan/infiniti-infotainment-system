import QtQuick
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
    readonly property bool hvacOff: (vs ? vs.fanSpeed : 3) === 0

    // --- Compact chips component ---
    component Chip: Surface {
        id: chip
        property string label
        property bool active: false
        property color activeColor: Theme.colors.accent
        property url iconSource: ""

        implicitWidth: label.length * 9 + 16 + (String(iconSource).length > 0 ? 20 : 0)
        implicitHeight: 22
        radius: 4
        color: active ? activeColor : Theme.colors.tile
        neomorph: false
        pressed: active

        RowLayout {
            anchors.centerIn: parent
            spacing: 4

            AppIcon {
                visible: String(chip.iconSource).length > 0
                source: chip.iconSource
                size: 14
                Layout.preferredWidth: 14
                Layout.preferredHeight: 14
                color: chip.active ? Theme.colors.onaccent : Theme.colors.textSecondary
            }

            Text {
                text: chip.label
                color: chip.active ? Theme.colors.onaccent : Theme.colors.textSecondary
                font {
                    family: Theme.typography.family
                    pixelSize: 8
                    bold: true
                    weight: Theme.typography.weightMedium
                }
            }
        }
    }

    // --- Fan bar (double size) ---
    component FanBars: Item {
        id: bars
        property int level: 3

        width: 18 * 7
        height: 40

        Repeater {
            model: 7
            Rectangle {
                width: 18
                height: 40
                x: index * 22
                y: 0
                radius: 4
                color: index < bars.level ? Theme.colors.accent : Theme.colors.sliderTrack
            }
        }
    }

    // --- Driver / Passenger temperature zone (wrap-content) ---
    component TempZone: Surface {
        property string zoneLabel: "Driver"
        property real tempValue: 21.5
        property bool dualMode: false
        property bool dimmed: false

        opacity: dimmed ? 0.4 : 1.0

        Layout.fillWidth: false
        Layout.fillHeight: true
        Layout.preferredWidth: tempText.implicitWidth + 32
        Layout.leftMargin: 4
        Layout.rightMargin: 4
        Layout.topMargin: 24
        Layout.bottomMargin: 24
        radius: Theme.metrics.cardRadius
        color: Theme.colors.surface
        neomorph: false

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - 16
            spacing: 1

                Text {
                    text: dualMode ? "Dual" : zoneLabel
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 14
                        bold: true
                        letterSpacing: 1
                        weight: Theme.typography.weightMedium
                    }
                    color: dualMode ? Theme.colors.success : Theme.colors.textSecondary
                }

                Text {
                    id: tempText
                    text: tempValue.toFixed(1) + "°C"
                    Layout.alignment: Qt.AlignHCenter
                    horizontalAlignment: Text.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 42
                        bold: true
                        weight: Font.DemiBold
                    }
                    color: Theme.colors.textPrimary
                }
            }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        // ====== ZONE 1: Driver temp only ======
        TempZone {
            zoneLabel: "Driver"
            tempValue: vs ? vs.driverTemp : 21.5
            dualMode: vs ? vs.dualMode : false
            dimmed: root.hvacOff
        }

        Rectangle {
            width: 1
            Layout.fillHeight: true
            color: Theme.colors.divider
        }

        // ====== ZONE 2: System Status ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: false
            opacity: root.hvacOff ? 0.4 : 1.0

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 4

                Text {
                    text: "CLIMATE"
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 14
                        bold: true
                        letterSpacing: 1.5
                        weight: Theme.typography.weightBold
                    }
                    color: Theme.colors.textSecondary
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true }

                // Rear defrost + airflow direction + fan speed
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter
                    spacing: 16

                    AppIcon {
                        source: Icons.rear
                        size: 40
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        color: (vs && vs.rearDefrostActive) ? Theme.colors.success : Theme.colors.sliderTrack
                    }

                    Rectangle {
                        width: 1
                        height: 40
                        color: Theme.colors.divider
                    }

                    AppIcon {
                        source: {
                            switch (vs ? vs.airflowMode : 0) {
                            case 1: return Icons.faceFeet;
                            case 2: return Icons.feet;
                            case 3: return Icons.windFeet;
                            default: return Icons.face;
                            }
                        }
                        size: 40
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                        color: Theme.colors.accent
                    }

                    Rectangle {
                        width: 1
                        height: 40
                        color: Theme.colors.divider
                    }

                    RowLayout {
                        visible: !root.hvacOff
                        spacing: 6

                        AppIcon {
                            source: (vs ? vs.fanSpeed : 3) >= 7 ? Icons.fanMax : Icons.fan
                            size: 40
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                            color: Theme.colors.accent
                        }

                        FanBars { level: vs ? vs.fanSpeed : 3 }
                    }

                    Text {
                        visible: root.hvacOff
                        text: "Off"
                        font {
                            family: Theme.typography.family
                            pixelSize: 24
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.textSecondary
                    }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true }

                // A/C / AUTO / RECIRC / ECO chips
                RowLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.bottomMargin: 8
                    spacing: 4

                    Chip { Layout.fillWidth: true; label: "A/C"; iconSource: Icons.ac; active: vs ? vs.acActive : true }
                    Chip { Layout.fillWidth: true; label: "AUTO"; iconSource: Icons.auto; active: vs ? vs.autoActive : true }
                    Chip { Layout.fillWidth: true; label: "RECIRC"; iconSource: Icons.rec; active: vs ? vs.recircActive : false }
                    Chip { Layout.fillWidth: true; label: "ECO"; iconSource: Icons.eco; active: vs ? vs.ecoMode : false }
                }
            }
        }

        Rectangle {
            width: 1
            Layout.fillHeight: true
            color: Theme.colors.divider
        }

        // ====== ZONE 3: Outside temp + Clock ======
        Surface {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: false

            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                spacing: 6

                TempZone {
                    zoneLabel: "Outside"
                    tempValue: vs ? vs.outsideTemp : 20
                }

                Rectangle {
                    width: 1
                    Layout.fillHeight: true
                    color: Theme.colors.divider
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    spacing: 2

                    Item { Layout.fillWidth: true; Layout.fillHeight: true }

                    Text {
                        text: clock ? clock.currentTime : "12:00"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 56
                            bold: true
                            weight: Font.DemiBold
                        }
                        color: Theme.colors.textPrimary
                    }

                    Text {
                        text: clock ? clock.currentDate : "MON 01"
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 24
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                        color: Theme.colors.textSecondary
                    }

                    Item { Layout.fillWidth: true; Layout.fillHeight: true }
                }
            }
        }

        Rectangle {
            width: 1
            Layout.fillHeight: true
            color: Theme.colors.divider
        }

        // ====== ZONE 4: Passenger temp only ======
        TempZone {
            zoneLabel: "Passenger"
            tempValue: vs ? vs.passengerTemp : 21.5
            dualMode: vs ? vs.dualMode : false
            dimmed: root.hvacOff
        }
    }
}
