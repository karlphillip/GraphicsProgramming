import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.9

Entity {
    id: root

    property Layer layer    
    property vector3d pos:  Qt.vector3d(0, 0, 0)
    property int width: 0
    property int height: 0
    property size meshResolution: Qt.size(2, 2)

    property Texture2D offscreenTexture // offscreen surface
    property bool invertYaxis: true

    //components: [ mesh, transform, material, root.layer ] // phong material
    components: [ mesh, transform, material2, root.layer ] // GLSL shaders

    PlaneMesh {
        id: mesh
        width: root.width
        height: root.height
        meshResolution: root.meshResolution
    }

    Transform {
        id: transform
        translation: Qt.vector3d(root.pos.x, root.pos.y, root.pos.z)
        rotationX: 90
    }

    PhongMaterial {
        id: material
        ambient: "orange"
        diffuse: "orange"
    }

    Material {
        id: material2

        parameters: [
            Parameter { name: "offscreenTexture"; value: root.offscreenTexture },
            Parameter { name: "invertYaxis"; value: root.invertYaxis }
        ]

        effect: Effect {
            techniques: [
                Technique {
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        majorVersion: 3
                        minorVersion: 2
                        profile: GraphicsApiFilter.CoreProfile
                    }

                    renderPasses: RenderPass {
                        shaderProgram: ShaderProgram {
                            vertexShaderCode: "
                                #version 150
                                in vec3 vertexPosition;
                                in vec2 vertexTexCoord;
                                out vec2 texCoord;
                                uniform mat4 mvp;
                                uniform bool invertYaxis;

                                void main()
                                {
                                    // pass texture coordinates to the fragment shader
                                    if (invertYaxis)
                                        texCoord = vec2(vertexTexCoord.s, 1.0 - vertexTexCoord.t);
                                    else
                                        texCoord = vec2(vertexTexCoord.s, vertexTexCoord.t);

                                    // Calculate the clip-space coordinates
                                    gl_Position = mvp * vec4(vertexPosition, 1.0);
                                }
                            "

                            fragmentShaderCode: "
                                #version 150
                                in vec2 texCoord;
                                uniform sampler2D offscreenTexture;
                                out vec4 fragColor;

                                void main()
                                {
                                    vec3 texColor = texture(offscreenTexture, texCoord).rgb;
                                    fragColor = vec4(texColor, 1.0);
                                }
                            "
                        }
                    }
                }
            ]
        }
    }
}
