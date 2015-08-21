#version 330

uniform sampler2D volume;
uniform sampler2D front;
uniform sampler2D back;

in vec2 vTexCoords;

out vec4 outputColor;

void main()
{
    vec4 frontTex = texture(front, vTexCoords);
    vec4 backTex = texture(back, vTexCoords);

    // if (frontTex.a > 0.0f || backTex.a > 0.0f) {
        vec3 frontPos = frontTex.xyz;
        vec3 backPos = backTex.xyz;
        outputColor = vec4(abs(vec3(backPos - frontPos)), 1.0f);
        // outputColor = vec4(abs(vec3(backPos)), 1.0f);
        // outputColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    // } else discard;
}
