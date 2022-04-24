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

    property color color: "green"
    //property alias alpha: material.alpha

    //components: [ mesh, transform, material, root.layer ]
    components: [ mesh, transform, materialGLSL, root.layer ]

    Mesh {
        id: mesh
        source: "qrc:/assets/low_poly_crystals.obj"
    }

    Transform {
        id: transform
        translation: Qt.vector3d(root.pos.x, root.pos.y, root.pos.z)
    }

    PhongMaterial {
        id: material
        ambient: Qt.rgba(0.19225, 0.19225, 0.19225, 1) // emitted by an object without any other light source.
        diffuse: root.color // emitted for rought surface reflections with the lights
        specular: Qt.rgba(0.508273, 0.508273, 0.508273, 1) // emitted for shiny surface reflections with the lights
        shininess: 51.2
        //alpha: 1.0
    }

    PhongShadingMaterial {
        id: materialGLSL
        ambient: Qt.rgba(0.19225, 0.19225, 0.19225, 1) // emitted by an object without any other light source.
        diffuse: root.color // emitted for rought surface reflections with the lights
        specular: Qt.rgba(0.508273, 0.508273, 0.508273, 1) // emitted for shiny surface reflections with the lights
        shininess: 51.2
        //alpha: 1.0
    }
}
