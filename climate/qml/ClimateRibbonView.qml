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
            spacing: 4
            anchors { fill: parent; leftMargin: 4; rightMargin: 4 }

            Item { Layout.fillWidth: true; Layout.fillHeight: true }

            AppIcon {
                visible: String(chip.iconSource).length > 0
                source: chip.iconSource
                size: 18
                color: chip.active ? Theme.colors.onaccent : Theme.colors.textSecondary
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                Layout.alignment: Qt.AlignVCenter
            }

            Text {
                text: chip.label
                color: chip.active ? Theme.colors.onaccent : Theme.colors.textSecondary
                Layout.alignment: Qt.AlignVCenter
                font {
                    family: Theme.typography.family
                    pixelSize: 8
                    bold: true
                    weight: Theme.typography.weightMedium
                }
            }

            Item { Layout.fillWidth: true; Layout.fillHeight: true }
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
        radius: Theme.metrics.cardRadius
        color: Theme.colors.surface
        neomorph: false
        Layout.fillWidth: false
        Layout.fillHeight: true
        Layout.preferredWidth: tempText.implicitWidth + 32
        Layout.leftMargin: 4
        Layout.rightMargin: 4
        Layout.topMargin: 24
        Layout.bottomMargin: 24

        ColumnLayout {
            anchors.centerIn: parent
            width: parent.width - 16
            spacing: 1

                Text {
                    text: dualMode ? "Dual" : zoneLabel
                    color: dualMode ? Theme.colors.success : Theme.colors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 14
                        bold: true
                        letterSpacing: 1
                        weight: Theme.typography.weightMedium
                    }
                }

                Text {
                    id: tempText
                    objectName: "tempText"
                    text: tempValue.toFixed(1) + "°C"
                    color: Theme.colors.textPrimary
                    horizontalAlignment: Text.AlignHCenter
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 42
                        bold: true
                        weight: Font.DemiBold
                    }
                }
            }
    }

    RowLayout {
        spacing: 6
        anchors.fill: parent
        anchors.margins: 8

        // ====== ZONE 1: Driver temp only ======
        TempZone {
            zoneLabel: "Driver"
            tempValue: vs ? vs.driverTemp : 21.5
            dualMode: vs ? vs.dualMode : false
            dimmed: root.hvacOff
        }

        Rectangle {
            color: Theme.colors.divider
            Layout.preferredWidth: 1
            Layout.fillHeight: true
        }

        // ====== ZONE 2: System Status ======
        Surface {
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: false
            opacity: root.hvacOff ? 0.4 : 1.0
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                spacing: 4
                anchors.fill: parent
                anchors.margins: 8

                Text {
                    text: "CLIMATE"
                    color: Theme.colors.textSecondary
                    Layout.alignment: Qt.AlignHCenter
                    font {
                        family: Theme.typography.family
                        pixelSize: 14
                        bold: true
                        letterSpacing: 1.5
                        weight: Theme.typography.weightBold
                    }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true }

                // Rear defrost + airflow direction + fan speed
                RowLayout {
                    spacing: 16
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter

                    AppIcon {
                        source: Icons.rear
                        size: 40
                        color: (vs && vs.rearDefrostActive) ? Theme.colors.success : Theme.colors.sliderTrack
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                    }

                    Rectangle {
                        color: Theme.colors.divider
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 40
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
                        color: Theme.colors.accent
                        Layout.preferredWidth: 40
                        Layout.preferredHeight: 40
                    }

                    Rectangle {
                        color: Theme.colors.divider
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 40
                    }

                    RowLayout {
                        visible: !root.hvacOff
                        spacing: 6

                        AppIcon {
                            source: (vs ? vs.fanSpeed : 3) >= 7 ? Icons.fanMax : Icons.fan
                            size: 40
                            color: Theme.colors.accent
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 40
                        }

                        FanBars { level: vs ? vs.fanSpeed : 3 }
                    }

                    Text {
                        visible: root.hvacOff
                        text: "Off"
                        color: Theme.colors.textSecondary
                        font {
                            family: Theme.typography.family
                            pixelSize: 24
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                    }
                }

                Item { Layout.fillWidth: true; Layout.fillHeight: true }

                // A/C / AUTO / RECIRC / ECO chips
                RowLayout {
                    spacing: 4
                    Layout.fillWidth: true
                    Layout.leftMargin: 24
                    Layout.rightMargin: 24
                    Layout.bottomMargin: 8

                    Chip { label: "A/C"; iconSource: Icons.ac; active: vs ? vs.acActive : true; Layout.fillWidth: true }
                    Chip { label: "AUTO"; iconSource: Icons.auto; active: vs ? vs.autoActive : true; Layout.fillWidth: true }
                    Chip { label: "RECIRC"; iconSource: Icons.rec; active: vs ? vs.recircActive : false; Layout.fillWidth: true }
                    Chip { label: "ECO"; iconSource: Icons.eco; active: vs ? vs.ecoMode : false; Layout.fillWidth: true }
                }
            }
        }

        Rectangle {
            color: Theme.colors.divider
            Layout.preferredWidth: 1
            Layout.fillHeight: true
        }

        // ====== ZONE 3: Outside temp + Clock ======
        Surface {
            radius: Theme.metrics.cardRadius
            color: Theme.colors.surface
            neomorph: false
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                spacing: 6
                anchors.fill: parent
                anchors.margins: 8

                TempZone {
                    zoneLabel: "Outside"
                    tempValue: vs ? vs.outsideTemp : 20
                }

                Rectangle {
                    color: Theme.colors.divider
                    Layout.preferredWidth: 1
                    Layout.fillHeight: true
                }

                ColumnLayout {
                    spacing: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Item { Layout.fillWidth: true; Layout.fillHeight: true }

                    Text {
                        text: clock ? clock.currentTime : "12:00"
                        color: Theme.colors.textPrimary
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 56
                            bold: true
                            weight: Font.DemiBold
                        }
                    }

                    Text {
                        text: clock ? clock.currentDate : "MON 01"
                        color: Theme.colors.textSecondary
                        Layout.alignment: Qt.AlignHCenter
                        font {
                            family: Theme.typography.family
                            pixelSize: 24
                            bold: true
                            weight: Theme.typography.weightMedium
                        }
                    }

                    Item { Layout.fillWidth: true; Layout.fillHeight: true }
                }
            }
        }

        Rectangle {
            color: Theme.colors.divider
            Layout.preferredWidth: 1
            Layout.fillHeight: true
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
