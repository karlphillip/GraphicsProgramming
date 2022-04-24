import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Entity {
    id: root

    property Layer layer
    property vector3d pos: Qt.vector3d(0, 0, 0)
    property alias width: mesh.width
    property alias height: mesh.height
    property alias x: transform.x
    property alias y: transform.y
    property alias z: transform.z
    property alias reflectionTexture: material.reflectionTexture
    property alias refractionTexture: material.refractionTexture    
    property alias refractionDepthTexture: material.refractionDepthTexture
    property alias waterViewProjection: material.waterViewProjection
    property alias dudvMap: material.dudvMap
    property alias cameraPosition: material.cameraPosition
    property alias normalMap: material.normalMap
    property alias lightColor: material.lightColor
    property alias lightPosition: material.lightPosition

    components: [ mesh, material, transform, root.layer ]

    PlaneMesh {
        id: mesh
        width: 20
        height: 20
    }

    WaterMaterial {
        id: material        
    }

    Transform {
        id: transform
        property int x: 0
        property int y: 0
        property int z: 0
        translation: Qt.vector3d(root.pos.x, root.pos.y, root.pos.z)
    }    
}
