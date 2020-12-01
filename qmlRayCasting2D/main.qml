/* A QML implementation of 2D raycasting.
 * Reference: https://www.youtube.com/watch?v=TOEi6T2mtHo
 */
import QtQuick 2.12
import QtQuick.Window 2.12
import "draw.js" as Draw

Window {
    id: window
    width: 600
    height: 600
    visible: true
    title: qsTr("2D Raytracing")
    color: "black"

    Canvas {
        id: canvas
        anchors.fill: parent

        property vector2d mousePoint: Qt.vector2d(0, 0)
        property var walls: createListOfWalls();
        property int numWalls: 5

        // a single light particle that emits rays on all directions
        Particle {
            id: particle
            pos: Qt.vector2d(canvas.mousePoint.x, canvas.mousePoint.y)
            numRays: 200
        }

        // drawing procedure
        onPaint: {
            // retrieve the drawing context: origin (0,0) is on the top-left
            var ctx = canvas.getContext("2d");

            // draw black background
            Draw.rect(ctx, 0, 0, window.width, window.height, "black");

            // draw walls
            for (let wall of walls)
                wall.show(ctx);

            // mouse click updates the particle position
            if (canvas.mousePoint.x || 0 && canvas.mousePoint.y > 0)
                particle.update(canvas.mousePoint.x, canvas.mousePoint.y);

            // draw particle that shines rays of light everywhere
            particle.show(ctx);

            // find if any of these rays in the particle are able to intercept the boundary
            //particle.lookWall(walls[0], ctx);
            particle.look(walls, ctx);
        }

        // refresh the canvas automatically every 33 milliseconds
        Timer {
            interval: 33
            running: true
            repeat: true
            onTriggered: canvas.requestPaint()
        }

        // createListOfWalls: creates a list of random walls on the screen
        function createListOfWalls() {
            var list = new Array(canvas.numWalls);

            for (let i = 0; i < canvas.numWalls; ++i) {
                var component = Qt.createComponent("Boundary.qml");
                if (component === null) {
                    console.log("Canvas !!! error creating component");
                    console.log(component.errorString());
                    return;
                }

                // invoke the constructor of Boundary with the following parameters
                var boundaryParams = {
                    "a": Qt.vector2d(Math.random()*window.width, Math.random()*window.height),
                    "b": Qt.vector2d(Math.random()*window.width, Math.random()*window.height)
                };
                var dynamicWall = component.createObject(canvas, boundaryParams);

                if (dynamicWall === null) {
                    console.log("Canvas !!! error creating object");
                    console.log(component.errorString());
                    return;
                }

                list[i] = dynamicWall;
            }

            return list;
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        //onClicked: {
        onPositionChanged: {
            canvas.mousePoint.x = mouse.x;
            canvas.mousePoint.y = mouse.y;
        }
    }
}
