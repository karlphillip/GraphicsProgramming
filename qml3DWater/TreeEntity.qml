import QtQuick 2.0 as QQ2
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root
    property Layer layer

    //
    // Base platform
    //

    Entity {
        components: [ platformMesh, platformTransform, platformMaterial, root.layer ]

        TorusMesh {
            id: platformMesh
            radius: 4
            minorRadius: 0.3
            rings: 60
            slices: 30
        }

        Transform {
            id: platformTransform
            translation: Qt.vector3d(-10, 7, 25)
            scale: 10

            QQ2.NumberAnimation on rotationY {
                from: 0
                to: 360
                duration: 4000
                loops: QQ2.Animation.Infinite
            }
        }

        // gold
        PhongShadingMaterial {
            id: platformMaterial
            ambient: Qt.rgba(0.24725, 0.1995, 0.0745, 1.0)
            diffuse: Qt.rgba(0.75164, 0.60648, 0.22648, 1.0)
            specular: Qt.rgba(0.628281, 0.555802, 0.366065, 1.0)
            shininess: 51.2

            lightPosition: sunLightEntity.position
            lightIntensity: Qt.vector3d(1.0, 1.0, 1.0)

            // Note: not defining a proper baseTexture here triggers this warning:
            //   Unknown uniform type or value: QVariant(std::nullptr_t, (nullptr)) Please check your QParameters
        }
    }

    //
    // Tree
    //

    Entity {
        components: [ treeMesh, treeTransform, treeMaterial, root.layer ]

        Mesh {
            id: treeMesh
            source: "qrc:/assets/tree.obj"
        }

        Transform {
            id: treeTransform
            translation: Qt.vector3d(-10, 0, 25)
            scale: 10
        }

        // gold
        PhongShadingMaterial {
            id: treeMaterial
            ambient: Qt.rgba(0.24725, 0.1995, 0.0745, 1.0)
            diffuse: Qt.rgba(0.75164, 0.60648, 0.22648, 1.0)
            specular: Qt.rgba(0.628281, 0.555802, 0.366065, 1.0)
            shininess: 51.2

            lightPosition: sunLightEntity.position
            lightIntensity: Qt.vector3d(1.0, 1.0, 1.0)

            // Note: not defining a proper baseTexture here triggers this warning:
            //   Unknown uniform type or value: QVariant(std::nullptr_t, (nullptr)) Please check your QParameters
        }
    }
}
