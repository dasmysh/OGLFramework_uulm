#version 450

uniform sampler3D volume;
uniform sampler1D transferFunc;
// uniform sampler2D back;
layout(rgba32f) uniform image2D back;
layout(rgba32f) uniform image2D colorAcc;
layout(r32f) uniform image2D overShootAdj;
uniform float lod = 0.0f;

in vec4 gl_FragCoord;
// layout(origin_upper_left) in vec4 gl_FragCoord;
// layout(pixel_center_integer) in vec4 gl_FragCoord;

in vec2 vTexPosition;
in vec3 vVolPosition;

layout(location = 0) out vec4 outputColor;

void main()
{
    ivec2 imgCoords = ivec2(gl_FragCoord.xy);
    vec4 backTex = imageLoad(back, imgCoords);
    // vec4 backTex = texture(back, vTexPosition);
    float overShoot = imageLoad(overShootAdj, imgCoords).x;

    // float stepSize = pow(2, lod) / 256.0f;
    float stepSize = pow(2, lod) / 512.0f;

    // Setup ray param
    vec3 rayStart = vVolPosition;
    vec3 rayDir = backTex.xyz - rayStart;

    float t1 = min(length(rayDir), length(vec3(1)));
    rayDir /= t1;

    // rayDir /= ((t1 > 0.0f) ? t1 : 1.0f);

    /*if (length(rayDir) >= length(vec3(1))) {
        outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        return;
    }*/

    // Reset Color and Alpha
    vec4 colorAccValue = imageLoad(colorAcc, imgCoords);
    vec3 C = colorAccValue.rgb;
    float A = colorAccValue.a;
    // vec3 C = vec3(0.0f);
    // float A = 0.0f;

    float t = overShoot;
    unsigned int numSteps = 0;
    while (t < t1 && A < 1.0) {
        // World Space Position
        vec3 p = rayStart + rayDir * t;

        // Intensity value of volume data
        // float s = textureLod(volume, p, lod).r;
        float s = texture(volume, p).r;

        /*vec4 color;
        if (s > 0.2f) color = vec4(s, s, s, 1.0f);
        else color = vec4(0.0f);*/

        // Transfer function lookup
        vec4 color = texture(transferFunc, s);
        color.a *= stepSize * 100;

        C += (1 - A) * color.a * color.rgb;
        A += (1 - A) * color.a;

        t += stepSize;
        ++numSteps;
    }

    //float fNumSteps = float(numSteps) / (512.0f * length(vec3(1)));
    // outputColor = vec4(fNumSteps, fNumSteps, fNumSteps, 1);
    //outputColor = vec4(rayDir, 1);
    // outputColor = vec4(backTex.xyz, 1);
    // Write Color to pixel
    //if (numSteps > 10) {
        colorAccValue = vec4(C, A);
        imageStore(colorAcc, imgCoords, colorAccValue);
        overShoot = t - t1;
        imageStore(overShootAdj, imgCoords, vec4(overShoot));
        // + (1 - A) * 0.1 *vec3(0,0,1)
        outputColor = vec4(C, 1);
    /*} else {
        outputColor = vec4(1, 0, 0, 1);
    }*/
}
