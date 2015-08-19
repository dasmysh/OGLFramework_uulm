#version 430

layout(rgba8) uniform image3D childTex;
layout(rgba8) uniform image3D combineTex;
uniform uvec3 childShift;
uniform uvec3 maxChunkSize;

layout (local_size_x = 8, local_size_y = 8, local_size_z = 8) in;
void main() {
    uvec3 storePos = uvec3(gl_GlobalInvocationID.xyz) + (maxChunkSize * childShift);
    uvec3 childSize = uvec3(imageSize(childTex));
    uvec3 combineSize = uvec3(imageSize(combineTex));
    if (storePos.x >= combineSize.x || storePos.y >= combineSize.y || storePos.z >= combineSize.z) return;
    vec3 ratio = vec3(childSize) / vec3(combineSize);

    uvec3 readBasePos = uvec3((vec3(gl_GlobalInvocationID.xyz) * ratio)) + childShift;
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
        vec3 val = imageLoad(childTex, clamp(readPos[i], ivec3(childShift), ivec(childSize - uvec3(1)))).xyz;
        avg += val.x;
        minimum = min(minimum, val.y);
        maximum = max(maximum, val.z);
    }
    avg /= 8.0f;
    imageStore(combineTex, clamp(storePos, ivec3(0), combineSize - ivec3(1)), vec4(avg, minimum, maximum, 0.0f));
}
