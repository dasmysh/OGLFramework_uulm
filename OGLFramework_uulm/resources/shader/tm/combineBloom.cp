#version 430

uniform sampler2D sourceTex;
uniform sampler2D blurTex[NUM_PASSES];
layout(rgba32f) uniform image2D targetTex;
uniform float defocus;
uniform float bloomIntensity;

vec4 pyramidFilter(sampler2D srcTex, vec2 tex, vec2 defocusWidth) {
    vec4 color = texture(srcTex, tex + vec2(0.5, 0.5) * defocusWidth);
    color += texture(srcTex, tex + vec2(-0.5,  0.5) * defocusWidth);
    color += texture(srcTex, tex + vec2( 0.5, -0.5) * defocusWidth);
    color += texture(srcTex, tex + vec2(-0.5, -0.5) * defocusWidth);
    return 0.25 * color;
}

const float w[7] = float[7](64.0, 32.0, 16.0, 8.0, 4.0, 2.0, 1.0);

layout(local_size_x = 32, local_size_y = 16, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 targetSize = imageSize(targetTex);

    if (pos.x >= targetSize.x || pos.y >= targetSize.y) return;

    ivec2 sourceSize = textureSize(sourceTex, 0);
    vec2 defocusWidth = vec2(defocus) / vec2(sourceSize);
    vec2 tex = vec2(pos) / vec2(targetSize);

    vec4 result = pyramidFilter(sourceTex, tex, defocusWidth);
    for (int i = 0; i < NUM_PASSES; i++) {
        vec4 passSmple = texture(blurTex[i], texcoord);
        result.rgb += bloomIntensity * w[i] * passSmple.rgb / 127.0;
        result.a += passSmple.a / N_PASSES;
    }
    imageStore(targetTex, pos, result);
}
