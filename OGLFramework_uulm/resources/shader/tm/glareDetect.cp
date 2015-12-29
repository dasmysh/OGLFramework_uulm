#version 430

uniform sampler2D sourceTex;
layout(rgba32f) uniform image2D targetTex;
uniform float exposure;
uniform float bloomThreshold;

const vec2 offsets[5] = vec2[5](vec2(0, 0), vec2(-1, 0), vec2(1, 0), vec2(0, -1), vec2(0, 1));

layout(local_size_x = 32, local_size_y = 16, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 targetSize = imageSize(targetTex);
    ivec2 sourceSize = textureSize(sourceTex, 0);
    vec2 pixelSize = vec2(1.0f) / vec2(sourceSize);

    if (pos.x >= targetSize.x || pos.y >= targetSize.y) return;

    vec4 color = vec4(1e20);
    vec2 tex = vec2(pos) / vec2(targetSize);
    for (int i = 0; i < 5; i++) {
        color = min(texture(sourceTex, tex + (offsets[i] * pixelSize)), color);
    }
        
    color.rgb *= exposure;
    vec4 result = vec4(max(color.rgb - vec3(bloomThreshold) / (1.0 - vec3(bloomThreshold)), 0.0), color.a);
    imageStore(targetTex, pos, result);
}
