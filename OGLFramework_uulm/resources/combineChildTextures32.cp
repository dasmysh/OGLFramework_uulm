#version 430

layout(rgba32f) uniform image3D origTex;
layout(rgba32f) uniform image3D mipTex;
uniform vec3 shift;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main() {
    ivec3 storePos = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 origSize = imageSize(origTex);
    ivec3 mipSize = imageSize(mipTex);
    vec3 ratio = vec3(origSize) / vec3(mipSize);

    ivec3 readBasePos = ivec3((vec3(storePos) * ratio) + shift);
    ivec3 readPos[8];
    readPos[0] = readBasePos + ivec3(0, 0, 0);
    readPos[1] = readBasePos + ivec3(0, 0, 1);
    readPos[2] = readBasePos + ivec3(0, 1, 0);
    readPos[3] = readBasePos + ivec3(0, 1, 1);
    readPos[4] = readBasePos + ivec3(1, 0, 0);
    readPos[5] = readBasePos + ivec3(1, 0, 1);
    readPos[6] = readBasePos + ivec3(1, 1, 0);
    readPos[7] = readBasePos + ivec3(1, 1, 1);

    float avg = 0.0f;
    float minimum = 1.0f;
    float maximum = 0.0f;
    for (int i = 0; i < 8; ++i) {
        vec3 val = imageLoad(origTex, clamp(readPos[i], ivec3(0), origSize)).xyz;
        avg += val.x;
        minimum = min(minimum, val.y);
        maximum = max(maximum, val.z);
    }
    avg /= 8.0f;
    imageStore(mipTex, storePos, vec4(avg, minimum, maximum, 0.0));
}
