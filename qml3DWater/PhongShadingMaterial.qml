import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Material {
    id: root

    // GLSL uniforms for phong shading model
    property color ambient: Qt.rgba(0.0, 0.0, 0.0)
    property color diffuse: Qt.rgba(0.0, 0.0, 0.0)
    property color specular: Qt.rgba(0.0, 0.0, 0.0)
    property real shininess: 0.0

    property vector3d lightPosition: Qt.vector3d(0.0, 0.0, 0.0)
    property vector3d lightIntensity: Qt.vector3d(1.0, 1.0, 1.0)

    property TextureLoader baseTexture
    property bool isBaseTextureLoaded: (baseTexture) ? true : false

    // define standard uniform variables (share data with the shaders)
    parameters: [
        Parameter { name: "ka"; value: Qt.vector3d(root.ambient.r, root.ambient.g, root.ambient.b) },
        Parameter { name: "kd"; value: Qt.vector3d(root.diffuse.r, root.diffuse.g, root.diffuse.b) },
        Parameter { name: "ks"; value: Qt.vector3d(root.specular.r, root.specular.g, root.specular.b) },
        Parameter { name: "shininess"; value: root.shininess },
        Parameter { name: "lightPosition"; value: root.lightPosition },
        Parameter { name: "lightIntensity"; value: root.lightIntensity },
        Parameter { name: "baseTexture"; value: root.baseTexture },
        Parameter { name: "isBaseTextureLoaded"; value: root.isBaseTextureLoaded }
    ]

    effect: Effect {        
       techniques: [           

           //
           // Profile for OpenGL 3.3 - NVIDIA GeForce GTX 1660 and NVIDIA RTX 3080
           //

           Technique {
               graphicsApiFilter {
                   api: GraphicsApiFilter.OpenGL
                   profile: GraphicsApiFilter.CoreProfile
                   majorVersion: 3
                   minorVersion: 3
               }

               ShaderProgram {
                   id: gl3shader
                   vertexShaderCode: loadSource("qrc:/shaders/terrain.vert")
                   fragmentShaderCode: loadSource("qrc:/shaders/terrain.frag")
               }


               renderPasses: [
                   //
                   // RenderPass #1: filters the framegraph for key "clippingPlane" with value 1
                   // for refraction
                   //

                   RenderPass {
                       filterKeys: [
                           FilterKey { name: "clippingPlane"; value: 1 }
                       ]

                       shaderProgram: gl3shader

                       parameters: [
                           // culls everything above the specified height
                           Parameter { name: "clipPlane"; value: Qt.vector4d(0.0, -1.0, 0.0, waterEntity.pos.y) }
                       ]
                   },

                   //
                   // RenderPass #2: filters the framegraph for key "clippingPlane" with value 2
                   // for reflection
                   //
                   RenderPass {
                       // filters the framegraph for key "clippingPlane" with value 2
                       filterKeys: [
                           FilterKey { name: "clippingPlane"; value: 2 }
                       ]

                       shaderProgram: gl3shader

                       parameters: [
                           // culls everything below the specified height
                           Parameter { name: "clipPlane"; value: Qt.vector4d(0.0, 1.0, 0.0, -waterEntity.pos.y + 2.0) }
                       ]
                   }
               ]

           } // Technique
       ] // techniques

   } // effect
}
