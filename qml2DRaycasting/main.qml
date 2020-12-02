/* A QML implementation of 2D raycasting.
 * Reference: https://www.youtube.com/watch?v=TOEi6T2mtHo
 */
import QtQuick 2.12
import QtQuick.Window 2.12
import "draw.js" as Draw

Window {
    id: window
    width: 800
    height: 800
    visible: true
    title: qsTr("2D Raycasting")
    color: "black"

    property bool showFps: true
    property int frameCounter: 0
    property real fps: 0.0

    Canvas {
        id: canvas
        anchors.fill: parent

        // change default render target to FBO: utilizes OpenGL hardware acceleration rather than rendering into system memory
        renderTarget: Canvas.FramebufferObject

        // change default render strategy to Threaded: context will defer graphics commands to a private rendering thread
        renderStrategy: Canvas.Threaded

        // store latest mouse move coordinates
        property vector2d mousePoint: Qt.vector2d(0, 0)

        // specify wall size and create walls
        property int numWalls: 5
        property var walls: createListOfWalls(numWalls);

        // a single light particle that emits rays on all directions
        Particle {
            id: particle
            pos: Qt.vector2d(canvas.mousePoint.x, canvas.mousePoint.y)
            numRays: 64
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

            // increment frame counter
            window.frameCounter++;
        }

        // try to refresh the canvas every 5ms
        Timer {
            interval: 5
            running: true
            repeat: true

            property double startTime: 0.0
            property double elapsedTime: 0.0
            property double msPerFrame: 0.0

            onTriggered: {
                if (startTime === 0)
                    startTime = new Date().getTime();

                elapsedTime = new Date().getTime() - startTime;

                if (elapsedTime >= 1000) {
                    // compute FPS
                    msPerFrame = elapsedTime / window.frameCounter;
                    window.fps = 1000 / msPerFrame;

                    // reset start time and fps count
                    startTime = 0;
                    window.frameCounter = 0;
                }

                // trigger canvas.onPaint()
                canvas.requestPaint()

                //console.log("FPS:" + fps);
            }
        }

        // createListOfWalls: creates a list of random walls on the screen
        function createListOfWalls(size) {
            var list = new Array(size);

            for (let i = 0; i < size; ++i) {
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

    Item {
        focus: true

        Keys.onPressed: {
            if (event.key === Qt.Key_F) {
                console.log("onPressed: F");
                showFps = !showFps;
                event.accepted = true;
            }
        }
    }


    Text {
        anchors.right: parent.right
        color: "lime"
        text: qsTr("FPS: " + window.fps.toFixed(1))
        visible: window.showFps
    }
}
