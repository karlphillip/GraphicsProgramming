import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property Layer layer
    property vector3d pos:  Qt.vector3d(0, 0, 0)

    property alias scale: transform.scale
    property alias rotationX: transform.rotationX
    property alias rotationY: transform.rotationY
    property alias rotationZ: transform.rotationZ

    property alias baseTexture: materialGLSL.baseTexture

    property color color: "green"
    //property alias alpha: material.alpha

//    components: [ mesh, transform, diffuseMapMaterial, root.layer ]
    components: [ mesh, transform, materialGLSL, root.layer ]

    Mesh {
        id: mesh
//        source: "qrc:/assets/minecraft/Forest.obj"
        source: "qrc:/assets/minecraft/ForestNoWater.obj"
    }

    Transform {
        id: transform
        translation: Qt.vector3d(root.pos.x, root.pos.y, root.pos.z)
    }

//    DiffuseMapMaterial {
//        id: diffuseMapMaterial
//        diffuse: TextureLoader { source: "qrc:/assets/minecraft/Forest.png" }
//    }

    PhongShadingMaterial {
        id: materialGLSL
        ambient: Qt.rgba(1.0, 1.0, 1.0, 1.0) // white
        baseTexture: TextureLoader { source: "qrc:/assets/minecraft/Forest.png" }
    }
}
