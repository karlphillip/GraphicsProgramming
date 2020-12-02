/* A QML implementation of a 2D raycasting engine which was used by Wolfenstein 3D.
 * Reference: https://www.youtube.com/watch?v=vYgIKn7iDH8
 */
import QtQuick 2.15
import QtQuick.Window 2.15
import "draw.js" as Draw

Window {
    id: window
    width: 800
    height: 400
    visible: true
    title: qsTr("2D Raycasting Engine")
    color: "black"

    property bool showFps: true
    property int frameCounter: 0
    property real fps: 0.0

    // a single light particle that emits rays on all directions
    Particle {
        id: particle
        pos: Qt.vector2d(200, 200)
        numRays: 50
    }

    // on this example, the canvas is split into two halfs: the first draws the raycasting and the second draws the world rendering
    Canvas {
        id: canvas
        anchors.fill: parent
        //antialiasing: true

        // change default render target to FBO: utilizes OpenGL hardware acceleration rather than rendering into system memory
        renderTarget: Canvas.FramebufferObject

        // change default render strategy to Threaded: context will defer graphics commands to a private rendering thread
        renderStrategy: Canvas.Threaded

        // define two windows for rendering where each occupy half the canvas
        property var sceneW: window.width / 2
        property var sceneH: window.height

        // create the labirynth
        property var walls: createListOfWalls();

        // drawing procedure
        onPaint: {            
            // retrieve the drawing context: origin (0,0) is on the top-left
            var ctx = canvas.getContext("2d");

            /* clear screen and draw both canvas */

            // draw black background on the left, canvas separator (vertical) and then white background on the right
            Draw.rect(ctx,      0, 0, sceneW, sceneH, "black");
            Draw.rect(ctx, sceneW, 0, sceneW, sceneH, "black");
            Draw.line(ctx, sceneW, 0, sceneW, sceneH, "red", 2); // lineWidth=2

            /* draw the maze */

            for (let wall of walls)
                wall.show(ctx);

            /* draw particle/player */

            particle.show(ctx);

            /* draw "3D walls" on the second canvas
             * note: the depth of a wall is given by scene[i] and so its the color
             */

            // find if any of the rays emitted by the particle is able to intercept a wall and
            // store those distances to the walls in the scene list
            const scene = particle.look(walls, ctx);
            const w = sceneW / scene.length; //            

            // push transform matrix and translate to the second canvas so that all subsequent drawing is made on the second canvas
            ctx.save();
            ctx.translate(sceneW, 0);

            for (let i = 0; i < scene.length; ++i) {
                //console.log("scene #" + i + " = " + scene[i]);

                const sq = scene[i] * scene[i];
                const wSq = sceneW * sceneW;

                // map the distance to an equivalent color: invert the value so that objects closest to the particle are brighter
                let c = mapRange(sq, 0, wSq, 255, 0) / 255.0;
                let grayShade = Qt.rgba(c, c, c, 1);

                // also, the further an object is, the shorter the height of its rect
                let h = mapRange(scene[i], 0, sceneW, sceneH, 0);

                // adjust the wall drawing so that its geometric center is in the middle the right canvas
                const rectX = (i*w+w/2) - w/2;
                const rectY = (sceneH/2) - h/2;
                Draw.rect(ctx, rectX, rectY, w+1, h, grayShade, false);
            }

            // pop transform matrix
            ctx.restore();

            // increment frame counter
            window.frameCounter++;
        }

        function mapRange(value, inMin, inMax, outMin, outMax) {
           return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
        }

        // refresh the canvas automatically every 5ms
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

        // createListOfWalls: creates a list of predefined walls on the screen
        function createListOfWalls() {
            var list = [];

            var component = Qt.createComponent("Boundary.qml");
            if (component === null) {
                console.log("Canvas !!! error creating component");
                console.log(component.errorString());
                return;
            }

            /* create edge walls: invoke the constructor of Boundary with the line segment (a,b) for each */

            list[0] = component.createObject(canvas, { "a": Qt.vector2d(0, 0), "b": Qt.vector2d(canvas.sceneW, 0) });
            list[1] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW, 0), "b": Qt.vector2d(canvas.sceneW, canvas.sceneH) });
            list[2] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW, canvas.sceneH), "b": Qt.vector2d(0, canvas.sceneH) });
            list[3] = component.createObject(canvas, { "a": Qt.vector2d(0, canvas.sceneH), "b": Qt.vector2d(0, 0) });

            /* create the internal walls of the maze */

            list[4] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.60, canvas.sceneH*0.15),
                                                       "b": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.15) });

            list[5] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.15),
                                                       "b": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.40) });

            list[6] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.60),
                                                       "b": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.85) });

            list[7] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.80, canvas.sceneH*0.85),
                                                       "b": Qt.vector2d(canvas.sceneW*0.60, canvas.sceneH*0.85) });


            list[8] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.20, canvas.sceneH*0.15),
                                                       "b": Qt.vector2d(canvas.sceneW*0.20, canvas.sceneH*0.40) });

            list[9] = component.createObject(canvas, { "a": Qt.vector2d(canvas.sceneW*0.20, canvas.sceneH*0.60),
                                                       "b": Qt.vector2d(canvas.sceneW*0.20, canvas.sceneH*0.85) });

            return list;
        }

    } // end of Canvas

    // handle keypresses
    Item {
        focus: true

        Keys.onPressed: {
            switch (event.key) {
                case Qt.Key_F:
                    console.log("onPressed: F");
                    window.showFps = !window.showFps;
                    event.accepted = true;
                    break;

                case Qt.Key_A:
                    console.log("onPressed: A");
                    particle.rotate(-0.1);
                    event.accepted = true;
                    break;

                case Qt.Key_D:
                    console.log("onPressed: D");
                    particle.rotate(0.1);
                    event.accepted = true;
                    break;

                case Qt.Key_W:
                    console.log("onPressed: W");
                    particle.move(10);
                    event.accepted = true;
                    break;

                case Qt.Key_S:
                    console.log("onPressed: S");
                    particle.move(-10);
                    event.accepted = true;
                    break;

                case Qt.Key_P:
                    particle.fisheye = !particle.fisheye;
                    event.accepted = true;
                    console.log("onPressed: P fisheye=" + particle.fisheye);
                    break;

                 default:
                     break;
            }
        }
    }

    // display FPS on the top-right corner of the application
    Text {
        anchors.right: parent.right
        color: "lime"
        text: qsTr("FPS: " + window.fps.toFixed(1))
        visible: window.showFps
    }
}
