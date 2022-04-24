#version 150 core
#ifdef GL_ES
precision highp float;
#endif
in vec3 worldPosition;
in vec3 worldNormal;
in vec2 texCoord;
in vec3 vertexColor;

out vec4 fragColor;

// Qt3D uniforms
uniform vec3 eyePosition;

// QML uniforms
uniform sampler2D baseTexture;
uniform bool isBaseTextureLoaded;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 ka;                // Ambient reflectivity
uniform vec3 kd;                // Diffuse reflectivity
uniform vec3 ks;                // Specular reflectivity
uniform float shininess;        // Specular shininess factor


vec3 adsModel(const in vec3 baseColor, const in vec3 vpos, const in vec3 vnormal)
{
    vec3 n = normalize(vnormal);
    vec3 s = normalize(lightPosition - vpos);
    float diffuse = max(dot(s, n), 0.0);

    float specular = 0.0;
    if (diffuse > 0.0 && shininess > 0.0) {
        vec3 r = reflect(-s, n);
        vec3 v = normalize(eyePosition - vpos);
        float normFactor = (shininess + 2.0) / 2.0;
        specular = normFactor * pow(max(dot(r, v), 0.0), shininess);
    }

    return lightIntensity * (ka * baseColor
                             + kd * diffuse * baseColor
                             + ks * specular);
}

void main()
{
    if (isBaseTextureLoaded)
    {
        // sample color from the texture and calculate Phong (ADS)
        vec4 baseColor = texture(baseTexture, texCoord);
        vec3 color = adsModel(baseColor.rgb, worldPosition, worldNormal);
        fragColor = vec4(color, baseColor.a);

//        fragColor = baseColor;
        return;
    }

    // otherwise, use the color calculated on the vertex shader
    fragColor = vec4(vertexColor, 1.0);
}
