import QtQuick 2.0
import "draw.js" as Draw

Item {
    property vector2d a: Qt.vector2d(0, 0)
    property vector2d b: Qt.vector2d(0, 0)

    function show(ctx) {
        Draw.line(ctx, a.x, a.y, b.x, b.y, Qt.rgba(255, 255, 255, 1), 1);

        //console.log("Boundary.show: a=" + a);
        //console.log("Boundary.show: b=" + b);
    }

    Component.onCompleted: {
        console.log("Boundary constructed: a=" + a + " b=" + b);
    }
}
