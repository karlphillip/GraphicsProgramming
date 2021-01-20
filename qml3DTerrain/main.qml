import QtQuick 2.12
import QtQuick.Scene3D 2.12
import Qt3D.Render 2.12
import Qt3D.Core 2.12
import Qt3D.Extras 2.12

Rectangle {
    id: window
    anchors.fill: parent
    color: "white"

    property var w: _window.width
    property var h: _window.height
    property var scale: 20

    MouseArea {
       id: mouseArea
       anchors.fill: parent
    }

    Scene3D {
        id: scene3d
        anchors.fill: parent
        focus: true
        aspects: ["input", "logic"]
        cameraAspectRatioMode: Scene3D.AutomaticAspectRatio
        multisample: false

        Entity {

            // !!! OrthographicProjection does not work this these shaders !!!
            Camera {
                id: mainCamera
                projectionType: CameraLens.PerspectiveProjection
                fieldOfView: 45
                aspectRatio: window.w / window.h
                nearPlane:   0.01
                farPlane:    1000.0

                position:   Qt.vector3d( 0.0, 0.0, 5.0 )
                viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
                upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
            }

            components: [
                RenderSettings {
                    activeFrameGraph: ForwardRenderer {
                        id: renderer
                        clearColor: "black"
                        camera: mainCamera
                    }
                }
            ]

            TerrainEntity {
                id: terrain
                cols: window.w / window.scale
                rows: window.h / window.scale
                yoff: 0

                /* animate the mesh uses robustwireframe.vert to create the elevations */

                SequentialAnimation {
                    loops: Animation.Infinite
                    running: true

                    NumberAnimation {
                        target: terrain;
                        property: "yoff";
                        duration: 60*1000; // 60sec
                        from: 0.0
                        to: 200.0
                    }
                }
            }
        }
    }
}
