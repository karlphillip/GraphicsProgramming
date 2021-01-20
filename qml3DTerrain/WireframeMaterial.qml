import Qt3D.Core 2.12
import Qt3D.Render 2.12

Material {
    id: root

    property color ambient: Qt.rgba( 0.05, 0.05, 0.05, 1.0 )
    property color diffuse: Qt.rgba( 0.7, 0.7, 0.7, 1.0 )
    property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
    property real shininess: 50.0
    property real lineWidth: 1.0
    property color lineColor: Qt.rgba( 1.0, 1.0, 1.0, 1.0 ) // set wireframe color: white

    // variable that controls Perlin noise in the vertex shader
    property real yoff: 0.0

    parameters: [
        Parameter { name: "ka"; value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter { name: "kd"; value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
        Parameter { name: "ksp"; value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess },
        Parameter { name: "line.width"; value: root.lineWidth },
        Parameter { name: "line.color"; value: root.lineColor },
        Parameter { name: "yoff"; value: root.yoff }
    ]
}
