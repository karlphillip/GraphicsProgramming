import QtQuick 2.12
import QtQuick.Window 2.12
import "draw.js" as Draw

Window {
    id: window
    title: qsTr("QuadTree")
    width: 400
    height: 400
    visible: true

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            canvas.mousePoint = Qt.vector2d(mouse.x, mouse.y);
            //console.log("onClicked:", canvas.mousePoint.x, ",", canvas.mousePoint.y);

            // LMB adds a new point to the QuadTree
            //canvas.draw(mouse.x, mouse.y);
        }

        onPositionChanged: {
            canvas.mousePoint = Qt.vector2d(mouse.x, mouse.y);
            //console.log("onPositionChanged:", canvas.mousePoint.x, ",", canvas.mousePoint.y);

            canvas.requestPaint();
        }
    }

    QuadTree {
        id: qtree
        boundary: Rect { x: 200; y: 200 ; w: 200; h: 200 } // x,y geographic center
        capacity: 4
    }

    Canvas {
        id: canvas
        anchors.fill: parent
        antialiasing: true

        // store latest mouse move coordinates
        property vector2d mousePoint: Qt.vector2d(0, 0)

        // onPaint: implements the drawing procedure
        onPaint: {
            // retrieve the drawing context: origin (0,0) is on the top-left
            var ctx = canvas.getContext("2d");

            var pointComponent = Qt.createComponent("Point.qml");
            if (pointComponent === null) {
                console.log("onPaint !!! error creating pointComponent");
                console.log(component.errorString());
                return;
            }

            /* initialize the QuadTree with random points */

            if (!qtree.points.length) {
                for (let i = 0; i < 500; ++i) {
                    let randX = Math.random() * window.width;
                    let randY = Math.random() * window.height;
                    let p = pointComponent.createObject(canvas, { "x": randX, "y": randY });

                    // add particle to the QuadTree
                    qtree.insert(p);

                    //console.log("onPaint: qtree.points #", i, "=", qtree.points[i].x, ",", qtree.points[i].y);
                }
            }

            // display QuadTree
            qtree.show(ctx);

            /* define a custom search area (green rect) using mouse coordinates */

            var rectComponent = Qt.createComponent("Rect.qml");
            if (rectComponent === null) {
                console.log("onPaint !!! error creating rectComponent");
                console.log(component.errorString());
                return;
            }

            // clicking and moving the mouse around sets a new position
            let searchArea = rectComponent.createObject(canvas, { "x": canvas.mousePoint.x, "y": canvas.mousePoint.y, "w": 75, "h": 50 });

            // display the search area
            Draw.rectCenter(ctx, searchArea.x, searchArea.y, searchArea.w*2, searchArea.h*2, "lime", "lime", false, 2);

            // find the points inside the search area
            let pointsFound = [];
            qtree.query(searchArea, pointsFound);
            console.log("onPaint: found", pointsFound.length, "points");

            // draw the points found
            for (let j = 0; j < pointsFound.length; ++j)
                Draw.circle(ctx, pointsFound[j].x, pointsFound[j].y, 3, "lime");
        }

        // draw: adds a new point to the QuadTree and invokes canvas.onPaint() to redraw the canvas
        function draw(mouseX, mouseY) {
            var pointComponent = Qt.createComponent("Point.qml");
            if (pointComponent === null) {
                console.log("draw !!! error creating pointComponent");
                console.log(component.errorString());
                return;
            }

            let p = pointComponent.createObject(canvas, { "x": mouseX, "y": mouseY });

            // add particle to the QuadTree
            qtree.insert(p);

            // trigger canvas.onPaint()
            canvas.requestPaint();
        }
    }
}
