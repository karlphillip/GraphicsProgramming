import QtQuick 2.0
import QtQuick.Layouts 1.15

Item {
    id: batteryId
    width: 210
    height: 300
    anchors.centerIn: parent

    property int charge: 100
    property int maxBars: 4
    property int bars: calcCharges()
    property string color: getColor()

    function getColor()
    {
        if (charge >= 75) return "#1FA900"; // green
        if (charge >= 50) return "#FF9C00"; // orange-ish
        if (charge >= 25) return "#D40027"; // darkred

        return "#AA2500"; // red
    }

    function calcCharges()
    {
        if (charge == 0)
            return 0;

        var numCharges = (charge / 100 * maxBars);
        //console.log("calcCharges: numCharges=", numCharges);

        if (numCharges > 1)
            return numCharges;

        return 1;
    }

    Rectangle {
        id: canvasId
        property int borderWidth: (0.08 * batteryId.width)
        property int chargeSpace: (0.07 * batteryId.width)

        // define the outter frame
        width: batteryId.width
        height: batteryId.height
        border.color: batteryId.color
        border.width: borderWidth
        radius: 20

        // Debug:
        //Component.onCompleted: print("canvasId: width=" + width + " height=" + height)

        // draw battery charges
        ColumnLayout {            
            spacing: canvasId.chargeSpace   // defines the space between the charges
            //Layout.alignment: Qt.AlignBottom
            anchors.bottom: canvasId.bottom
            anchors.bottomMargin: canvasId.borderWidth + canvasId.chargeSpace
            anchors.horizontalCenter: canvasId.horizontalCenter // ColumnLayout looses anchors upon redraw

            Repeater {
                model: batteryId.bars

                // each charge size is calculated dinamically
                Rectangle {
                    color: batteryId.color
                    width: canvasId.width - (canvasId.borderWidth * 4);
                    height: (canvasId.height - ((canvasId.borderWidth + canvasId.chargeSpace)*2) - (canvasId.chargeSpace * (batteryId.maxBars-1))) / batteryId.maxBars;

                    // Debug:
                    //Component.onCompleted: print("Rectangle: charge=" + batteryId.charge + " bars=" + batteryId.bars + " x:" + x + " y:" + y + " height:" + height)
                }
            } // Repeater
        } // ColumnLayout
    } // Rectangle
} // Item
