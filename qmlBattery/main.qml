/*
 * A demo that displays a battery widget in QML. The desired amount of charge can be set through the "charge" property.
 * The number of bars displayed set automatically the color of the widget. On this demo, a new charge value is randomly
 * set by the C++ application every 2 seconds.
 */
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import com.company.batterycontroller 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Battery Demo")

    BatteryController {
        id: batteryControllerId
    }

    Text {
        text: batteryId.charge + "%"
        color: batteryId.color
        anchors.horizontalCenter: parent.horizontalCenter
        font.pointSize: 20
    }

    Battery {
        id: batteryId

        // battery charge can be set to a staticvalue in QML or dinamically with BatteryController in C++
        //charge: 50
        charge: batteryControllerId.chargeValue

        // additional settings:
        //color : "blue"
        maxBars: 4
    }    
}
