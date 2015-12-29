#version 430

uniform sampler2D sourceTex;
layout(rgba32f) uniform image2D targetTex;
uniform vec2 dir;
uniform float bloomWidth;

const float offsets[13] = float[13](-1.7688, -1.1984, -0.8694, -0.6151, -0.3957, -0.1940, 0, 0.1940, 0.3957, 0.6151, 0.8694, 1.1984, 1.7688);

layout(local_size_x = 32, local_size_y = 16, local_size_z = 1) in;
void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 targetSize = imageSize(targetTex);

    if (pos.x >= targetSize.x || pos.y >= targetSize.y) return;

    ivec2 sourceSize = textureSize(sourceTex, 0);
    vec2 dirPixels = (dir / vec2(sourceSize)) * bloomWidth;
    vec2 tex = vec2(pos) / vec2(targetSize);

    vec4 color = vec4(0.0f);
    for (int i = 0; i < 13; i++)
        color += texture(sourceTex, tex + (dirPixels * offsets[i]));
    imageStore(targetTex, pos, color / 13.0f);
}
