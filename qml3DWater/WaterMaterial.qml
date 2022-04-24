// Enabling clipping planes on the framegraph is essential to be able to set gl_ClipDistance[0] and have
// OpenGL use it for vertex culling.
//
// This tells us how far a vertex is from the clipping plane 0. OpenGL only cares about the signed distance
// of each vertex from that plane wherever that plane may be:
// https://www.youtube.com/watch?v=0NH9k4zTAqk&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh&index=3
//
// Imagine a clipping plane separating whats below water and whats above water level:
//  - The distance is positive when the vertex is inside of the plane (below water level);
//  - The distance is negative when the vertex is outside the plane (above water level, the culling side of the plane);
//
// Since this is done for every vertex, and OpenGL interpolates these distances to the whole Geometry
// and any part of the geometry with a negative distnace is culled and therefore its not rendered, while
// anything with a distance of 0.0 or bigger than 0.0 is rendered.
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

Material {
    id: root

    // used to render the refraction and reflection on the water surface
    property Texture2D reflectionTexture
    property Texture2D refractionTexture    
    property matrix4x4 waterViewProjection

    // used to add some distortion on reflection/refraction on the water surface
    property TextureLoader dudvMap

    // used for fresnel effect
    property vector3d cameraPosition

    // used to create specular highlights
    property TextureLoader normalMap
    property vector3d lightColor
    property vector3d lightPosition

    //
    property Texture2D refractionDepthTexture

    parameters: [
        Parameter { name: "reflectionTex"; value: root.reflectionTexture },
        Parameter { name: "refractionTex"; value: root.refractionTexture },
        Parameter { name: "refractionDepthTex"; value: root.refractionDepthTexture },
        Parameter { name: "waterViewProjection"; value: root.waterViewProjection },
        Parameter { name: "dudvMap"; value: root.dudvMap },
        Parameter { name: "cameraPosition"; value: root.cameraPosition },
        Parameter { name: "normalMap"; value: root.normalMap },
        Parameter { name: "lightColor"; value: root.lightColor },
        Parameter { name: "lightPosition"; value: root.lightPosition }
    ]

    effect: Effect {
        FilterKey {
            id: forward
            name: "renderingStyle"
            value: "forward"
        }

        ShaderProgram {
            id: gl3Shader
            vertexShaderCode: loadSource("qrc:/shaders/water.vert")
            fragmentShaderCode: loadSource("qrc:/shaders/water.frag")
        }

        techniques: [
            //
            // OpenGL 3.2+ (Desktop)
            //
            Technique {
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 3
                    minorVersion: 2
                }

                renderPasses: [
                    RenderPass {
                        filterKeys: [ forward ]
                        shaderProgram: gl3Shader                        
                    }
                ]
            }
        ]
    }
}
