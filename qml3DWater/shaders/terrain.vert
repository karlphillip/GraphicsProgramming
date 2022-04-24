#version 150 core
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

out vec2 texCoord;
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 worldView;
out vec3 vertexColor;

uniform vec3 ka;            // Ambient reflectivity
uniform vec3 kd;            // Diffuse reflectivity
uniform vec3 ks;            // Specular reflectivity
uniform float shininess;    // Specular shininess factor
uniform vec3 eyePosition;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewProjection;

// (A,B,C,D) represents the horizontal plane's normal and the Distance (height = water.position.y)
uniform vec4 clipPlane; // vec4(0.0, -1.0, 0.0, 55.0);

#pragma include phong.inc.frag


void main()
{
    // pass through
    texCoord = vertexTexCoord;
    vec3 worldNormal = normalize(modelNormalMatrix * vertexNormal);
    vec3 worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

    // calculate Phong on the vertex shader
    worldView = normalize(eyePosition - worldPosition);
    vec3 diffuseColor, specularColor;
    adsModel(worldPosition, worldNormal, worldView, shininess, diffuseColor, specularColor);
    vertexColor = ka + kd * diffuseColor + ks * specularColor;

    // output vertex position
    gl_Position = modelViewProjection * vec4(vertexPosition, 1.0);

    // calculate the distance between this vertex and the horizontal clipping plane:
    // When gl_ClipDistance[0] = -1. the vertex is clipped and therefore its not rendered
    vec4 worldPos = modelMatrix * vec4(vertexPosition, 1.0);
    gl_ClipDistance[0] = dot(worldPos, clipPlane);
}
