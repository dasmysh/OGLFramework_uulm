
#define M_PI 3.1415926535897932384626433832795f

struct Light
{
    vec4 position;
    vec4 direction;
    vec4 intensity;
    float angFalloffStart;
    float angFalloffWidth;
    float distAttenuation;
    float farZ;
};

layout(std140) uniform lightsBuffer
{
    Light lights[NUM_LIGHTS];
};

vec3 lightIntensity(vec3 position, vec3 normal) {
    vec3 intensity = vec3(0.0f);
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        vec3 light = vec3(lights[i].position) - position;
        float lDist = length(light);
        light /= lDist;

        float cosTerm = clamp(dot(light, normal), 0.0f, 1.0f);

        vec3 lambert = (cosTerm * vec3(lights[i].intensity)) / M_PI;
        intensity += lambert;
    }
    return intensity;
}
