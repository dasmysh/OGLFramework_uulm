#version 330

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in uint passIndex[];

smooth out vec2 vTexCoord;
flat out float page;

struct glyph_info {
    vec4 pos_offset;
    vec4 hTex_hPixel_aspect_page;
};

layout(std140) uniform fontMetrics
{
    glyph_info gi[96];
} fm;

layout(std140) uniform orthoProjection {
    mat4 toScreen;
} ortho_rendering;

uniform vec4 fontStyle;
uniform vec4 fontPos;
uniform sampler2DArray fontTex;

void main() {
    uint pi = passIndex[0];
    vec2 dimTex = textureSize(fontTex, 0).xy;
    vec4 dirUp = normalize(vec4(-fontPos.w, fontPos.z, 0, 0));
    vec4 dir = normalize(vec4(fontPos.zw, 0, 0));
    mat2 dirMat = mat2(dir.xy, dirUp.xy);
    vec2 offset = dirMat * (fm.gi[pi].pos_offset.zw * fontStyle.zw);
    vec2 baseTex = fm.gi[pi].pos_offset.xy;
    float hP = fm.gi[pi].hTex_hPixel_aspect_page.y;
    float wP = hP * fm.gi[pi].hTex_hPixel_aspect_page.z;
    float hT = fm.gi[pi].hTex_hPixel_aspect_page.x;
    float wT = wP / dimTex.x;
    vec2 sizeTex = vec2(wT, hT);
    vec2 sizeScreen = vec2(wP, hP) * fontStyle.zw;

    // posBase: position on baseline
    vec2 posBase = gl_in[0].gl_Position.xy + fontPos.xy;
    // posTopLeft: position in top left corner
    vec4 posTopLeft = vec4(posBase + offset, gl_in[0].gl_Position.z, 1);
    // posTopLeft + dir*shearing
    vec4 pos1 = posTopLeft + dir * fontStyle.y;
    // posTopLeft + height
    vec4 pos2 = posTopLeft + (dirUp * sizeScreen.y);
    // pos1 + width
    vec4 pos3 = pos1 + (dir * sizeScreen.x);
    // pos2 + width
    vec4 pos4 = pos2 + (dir * sizeScreen.x);

    gl_Position = ortho_rendering.toScreen * pos1;
    page = fm.gi[pi].hTex_hPixel_aspect_page.w;
    vTexCoord = vec2(baseTex.x, -baseTex.y);
    EmitVertex();

    gl_Position = ortho_rendering.toScreen * pos2;
    page = fm.gi[pi].hTex_hPixel_aspect_page.w;
    vTexCoord = vec2(baseTex.x, -(baseTex.y + sizeTex.y));
    EmitVertex();

    gl_Position = ortho_rendering.toScreen * pos3;
    page = fm.gi[pi].hTex_hPixel_aspect_page.w;
    vTexCoord = vec2(baseTex.x + sizeTex.x, -baseTex.y);
    EmitVertex();

    gl_Position = ortho_rendering.toScreen * pos4;
    page = fm.gi[pi].hTex_hPixel_aspect_page.w;
    vTexCoord = vec2(baseTex.x + sizeTex.x, -(baseTex.y + sizeTex.y));
    EmitVertex();
}
