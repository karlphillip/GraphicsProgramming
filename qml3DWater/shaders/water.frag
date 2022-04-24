#version 150 core
#ifdef GL_ES
precision highp float;
#endif
in vec2 tc;
in vec4 clipSpace;
in vec4 positionInWaterSpace;
in vec3 toCameraVector; // from water to camera
in vec3 fromLightVector; // from light to water

out vec4 fragColor;

// Qt3D default uniforms
uniform float time; // nano seconds

// QML uniforms
uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D refractionDepthTex;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform vec3 lightColor;

// global constants
const float waveStrength = 0.04;    // strength of the dudvMap distortion
const float waveSpeed = 0.03;       // how fast the wave moves
const float shineDamper = 20.0;     // control how many specular highlights there are
const float reflectivity = 0.5;     // how bright the specular highlights are


// reflection + refraction
void method1()
{
    //
    // calculate the reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 projReflectionCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);
    vec4 reflectionColor = texture(reflectionTex, projReflectionCoords.xy);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    //
    // define final color
    //

    // final effect: mix reflection and refraction colors
    fragColor = mix(reflectionColor, refractionColor, 0.5);
}

// reflection + refraction + blue tint
void method2()
{
    //
    // calculate the reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 projReflectionCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);
    vec4 reflectionColor = texture(reflectionTex, projReflectionCoords.xy);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    //
    // define final color
    //

    // final effect: add some blue to the final color
    fragColor = mix(reflectionColor, refractionColor, 0.5);
    fragColor = mix(fragColor, vec4(0.0, 0.5, 1.0, 1.0), 0.5);
}

// reflection + refraction + blue tint + dudv distortion
void method3()
{
    //
    // calculate the projected reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 reflectTexCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;

    //
    // sample DuDv map to add distortions to the water surface
    //

    // make the distortion move to achieve a wave effect
    float moveFactor = (time * waveSpeed);

    //vec2 distortion1 = (texture(dudvMap, vec2(tc.x, tc.y)).rg * 2.0 - 1.0) * waveStrength; // no wave
    vec2 distortion1 = (texture(dudvMap, vec2(tc.x + moveFactor, tc.y)).rg * 2.0 - 1.0) * waveStrength; // wave on tc.x
    vec2 distortion2 = (texture(dudvMap, vec2(-tc.x + moveFactor, tc.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength; // wave on tc.x and tc.y
    vec2 totalDistortion = distortion1 + distortion2;

    reflectTexCoords += totalDistortion;
    refractTexCoords += totalDistortion;

    // clamp the coordinates to minimize the phantom artifact near the edge of the lake
    reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    //
    // define final color
    //

    vec4 reflectionColor = texture(reflectionTex, reflectTexCoords);
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    // final effect: add some blue to the final color
    fragColor = mix(reflectionColor, refractionColor, 0.5);
    fragColor = mix(fragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}

// reflection + refraction + blue tint + dudv distortion + fresnel effect
void method4()
{
    //
    // calculate the projected reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 reflectTexCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;

    //
    // sample DuDv map to add distortions to the water surface
    //

    // make the distortion move to achieve a wave effect
    float moveFactor = (time * waveSpeed);

    //vec2 distortion1 = (texture(dudvMap, vec2(tc.x, tc.y)).rg * 2.0 - 1.0) * waveStrength; // no wave
    vec2 distortion1 = (texture(dudvMap, vec2(tc.x + moveFactor, tc.y)).rg * 2.0 - 1.0) * waveStrength; // wave on tc.x
    vec2 distortion2 = (texture(dudvMap, vec2(-tc.x + moveFactor, tc.y + moveFactor)).rg * 2.0 - 1.0) * waveStrength; // wave on tc.x and tc.y
    vec2 totalDistortion = distortion1 + distortion2;

    reflectTexCoords += totalDistortion;
    refractTexCoords += totalDistortion;

    // clamp the coordinates to minimize the phantom artifact near the edge of the lake
    reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    //
    // define final color
    //

    vec4 reflectionColor = texture(reflectionTex, reflectTexCoords);
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    // fresnel effect: normalize it because the dot product needs a vector betwen [0.0, 1.0]
    vec3 viewVector = normalize(toCameraVector); // distance between the water surface and the camera
    float refractiveVector = dot(viewVector, vec3(0.0, 1.0, 0.0)); // how transparent the water should be
    refractiveVector = pow(refractiveVector, 0.5); // decrease the strength of effect

    // final effect: add some blue to the final color
    //fragColor = mix(reflectionColor, refractionColor, 0.5);
    fragColor = mix(reflectionColor, refractionColor, refractiveVector);
    fragColor = mix(fragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}

// reflection + refraction + blue tint + dudv distortion + fresnel effect + normal map for specular highlights
void method5()
{
    //
    // calculate the projected reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 reflectTexCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;

    //
    // sample DuDv map to add distortions to the water surface
    //

    // make the distortion move to achieve a wave effect
    float moveFactor = (time * waveSpeed);

    // new code, faster
    vec2 distortedTexCoords = texture(dudvMap, vec2(tc.x + moveFactor, tc.y)).rg * 0.1;
    distortedTexCoords = tc + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

    reflectTexCoords += totalDistortion;
    refractTexCoords += totalDistortion;

    // clamp the coordinates to minimize the phantom artifact near the edge of the lake
    reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    //
    // define final color
    //

    vec4 reflectionColor = texture(reflectionTex, reflectTexCoords);
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    // calculate fresnel effect: normalize it because the dot product needs a vector betwen [0.0, 1.0]
    vec3 viewVector = normalize(toCameraVector); // distance between the water surface and the camera
    float refractiveVector = dot(viewVector, vec3(0.0, 1.0, 0.0)); // how transparent the water should be
    refractiveVector = pow(refractiveVector, 0.5); // decrease the strength of effect

    // calculate specular highlights: retrieve the normal for this point from the normal map
    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0); // "* 2 - 1" converts it into [-1.0, +1.0]
    normal = normalize(normal); // convert into unit vector

    // calculate specular highlights: measure how much light its going into the camera lens
    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlights = lightColor * specular * reflectivity;

    // final effect: add some blue to the final color
    fragColor = mix(reflectionColor, refractionColor, refractiveVector);
    fragColor = mix(fragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) ;
    fragColor += vec4(specularHighlights, 0.0);
}

// reflection + refraction + blue tint + dudv distortion + fresnel effect + normal map for specular highlights
// + soft edges
void method6()
{
    //
    // calculate the projected reflection coords:
    // positionInWaterSpace is already multiplied by the biasMatrix to allow it to be in texture range [0.0, 1.0]
    // after perspective division which converts the vertex from Clip Space to NDC:
    vec2 reflectTexCoords = (positionInWaterSpace.xy / positionInWaterSpace.w);

    //
    // calculate the refraction coords:
    // here, perspective division converts Clip Space to NDC which results in the coordinate range [-1, +1]:
    //       1
    //       |
    // -1 ------- 1
    //       |
    //      -1
    vec2 ndc = clipSpace.xy / clipSpace.w;

    // and then convert this NDC coordinate to the texture coordinate range [0.0, 1.0]:
    // (0,1)       (1, 1)
    //      |
    //      |
    //      |______
    // (0,0)       (1, 0)
    vec2 refractTexCoords = ndc / 2.0 + 0.5;

    //
    // use the depth of the water to fix the glitch on the edges of the water
    //

    float znear = 0.01;     // mainCamera.nearPlane
    float zfar = 10000.0;   // mainCamera.farPlane
    float depth = texture(refractionDepthTex, refractTexCoords).r;

    // distance from camera to the bottom of the terrain under water (depth is linearized)
    float floorDistance = 2.0 * znear * zfar / (zfar + znear - (depth * 2.0 - 1.0) * (zfar - znear));

    // distance from camera to the water surface: gl_FragCoord.z (depth is linearized)
    float waterDistance = 2.0 * znear * zfar / (zfar + znear - (gl_FragCoord.z * 2.0 - 1.0) * (zfar - znear));

    float waterDepth = floorDistance - waterDistance;

    //
    // sample DuDv map to add distortions to the water surface
    //

    // make the distortion move to achieve a wave effect
    float moveFactor = (time * waveSpeed);

    // new code, faster
    vec2 distortedTexCoords = texture(dudvMap, vec2(tc.x + moveFactor, tc.y)).rg * 0.1;
    distortedTexCoords = tc + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
    //vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength * clamp(waterDepth / 20.0, 0.0, 1.0); // fix glitchy edges

    reflectTexCoords += totalDistortion;
    refractTexCoords += totalDistortion;

    // clamp the coordinates to minimize the phantom artifact near the edge of the lake
    reflectTexCoords = clamp(reflectTexCoords, 0.001, 0.999);
    refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);

    // prerequisite for specular highlights: retrieve the normal for this point from the normal map
    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0); // "* 2 - 1" converts it into [-1.0, +1.0]
    normal = normalize(normal); // convert into unit vector

    //
    // fresnel effect
    //

    // calculate fresnel effect: normalize it because the dot product needs a vector betwen [0.0, 1.0]
    vec3 viewVector = normalize(toCameraVector); // distance between the water surface and the camera
    //float refractiveVector = dot(viewVector, vec3(0.0, 1.0, 0.0)); // how transparent the water should be
    float refractiveVector = dot(viewVector, normal); // how transparent the water should be
    refractiveVector = pow(refractiveVector, 0.5); // decrease the strength of effect

    //
    // specular highlights using normal map
    //

    // calculate specular highlights: measure how much light its going into the camera lens
    vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
    float specular = max(dot(reflectedLight, viewVector), 0.0);
    specular = pow(specular, shineDamper);
    //vec3 specularHighlights = lightColor * specular * reflectivity;
    vec3 specularHighlights = lightColor * specular * reflectivity * clamp(waterDepth / 5.0, 0.0, 1.0);

    //
    // calculate final color
    //

    vec4 reflectionColor = texture(reflectionTex, reflectTexCoords);
    vec4 refractionColor = texture(refractionTex, refractTexCoords);

    // final effect: add some blue to the final color
    fragColor = mix(reflectionColor, refractionColor, refractiveVector);
    fragColor = mix(fragColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) ;
    fragColor += vec4(specularHighlights, 0.0);

    // at a water depth of 5.0, the alpha is set to 1.0
    fragColor.a = clamp(waterDepth / 5.0, 0.0, 1.0);
//    fragColor = vec4(waterDepth / 50.0); // DEBUG: render depth bufer
}

void main()
{
    // reflection + refraction
//    method1();
//    return;

    // reflection + refraction + blue tint
//    method2();
//    return;

    // reflection + refraction + blue tint + DuDv distortion
//    method3();
//    return;

    // reflection + refraction + blue tint + dudv distortion + fresnel effect
//    method4();
//    return;

    // reflection + refraction + blue tint + dudv distortion + fresnel effect + specular highlights
//    method5();
//    return;

    // reflection + refraction + blue tint + dudv distortion + fresnel effect + specular highlights + soft edges
    method6();
}
