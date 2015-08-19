#version 430

layout(r16) uniform image3D origTex;
layout(rgba16f) uniform image3D minMaxTex;

layout(local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main() {
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 origSize = imageSize(origTex);
    ivec3 minMaxSize = imageSize(minMaxTex);
    if (storePos.x >= minMaxSize.x || storePos.y >= minMaxSize.y || storePos.z >= minMaxSize.z) return;

    float val = imageLoad(origTex, clamp(storePos, ivec3(0), origSize - ivec3(1))).x;
    imageStore(minMaxTex, clamp(storePos, ivec3(0), minMaxSize - ivec3(1)), vec4(val, val, val, 0.0f));
}
