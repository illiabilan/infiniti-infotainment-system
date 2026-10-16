import QtQuick
import QtTest

import "../climate/qml"

Item {
    id: root
    width: 1280
    height: 160

    Component {
        id: ribbonComponent
        ClimateRibbonView {}
    }

    TestCase {
        name: "ClimateRibbonViewTests"
        when: windowShown

        function test_componentExists() {
            let ribbon = createTemporaryObject(ribbonComponent, root)
            verify(!!ribbon, "Component exists")
        }

        function test_hvacOffDefault() {
            let ribbon = createTemporaryObject(ribbonComponent, root)
            verify(!!ribbon, "Component exists")
            compare(ribbon.hvacOff, false)
        }

        function test_driverTempDefault() {
            let ribbon = createTemporaryObject(ribbonComponent, root)
            verify(!!ribbon, "Component exists")
            let tempText = findChild(ribbon, "tempText")
            verify(!!tempText, "Object exists")
            compare(tempText.text, qsTr("21.5°C"))
        }
    }
}
