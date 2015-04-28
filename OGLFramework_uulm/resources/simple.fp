#version 330

layout(std140) uniform colors
{
    vec3 color;
};

out vec4 outputColor;
void main()
{
   outputColor = vec4(color.x, color.y, color.z, 1.0f);
}
