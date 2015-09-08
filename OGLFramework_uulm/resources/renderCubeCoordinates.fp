#version 330

in vec2 vTexPosition;
in vec3 vVolPosition;

layout(location = 0) out vec4 outputColor;

void main()
{
    outputColor = vec4(vVolPosition, 1.0f);
}
