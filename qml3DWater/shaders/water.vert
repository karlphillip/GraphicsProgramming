#version 150 core
in vec3 vertexPosition;
in vec2 vertexTexCoord;

out vec2 tc;
out vec4 clipSpace;
out vec4 positionInWaterSpace;
out vec3 toCameraVector; // from water to camera
out vec3 fromLightVector; // from light to water

// Qt3D default uniforms
uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// QML uniforms
uniform mat4 waterViewProjection;
uniform vec3 cameraPosition;
uniform vec3 lightPosition; // sun position

// global constants
const float tiling = 4.0; // tile the DuDV map

void main()
{   
    // calculate clip space coordinates
    gl_Position = mvp * vec4(vertexPosition, 1.0);

    // forward the Clip Space coords to the Fragment Shader for projective texture mapping
    clipSpace = gl_Position;

    // forward the vertex coordinates
    tc = vertexTexCoord.xy * tiling;

    // distance between this vertex on the water surface and the camera
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1.0);
    toCameraVector = cameraPosition - worldPosition.xyz;

    // distance between this vertex on the water surface and the light
    fromLightVector = worldPosition.xyz - lightPosition;

    // forward fragment position as if it were in Water Space (reflection)
    //
    // multiplying the vertex by waterViewProjection transforms it into homogeneous coordinates [-1, 1]
    // however, texture sampling must happen between [0, 1].
    //
    // this means that a fragment in the middle of the screen will be at (0,0) in homogeneous coords
    // but since it will have to sample the middle of the texture, the UVs will have to be (0.5, 0.5)
    const mat4 biasMatrix = mat4(0.5, 0.0, 0.0, 0.0,
                                 0.0, 0.5, 0.0, 0.0,
                                 0.0, 0.0, 0.5, 0.0,
                                 0.5, 0.5, 0.5, 1.0);

    positionInWaterSpace = biasMatrix * waterViewProjection * modelMatrix * vec4(vertexPosition, 1.0);
}
