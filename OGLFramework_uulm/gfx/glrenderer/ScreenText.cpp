/**
 * @file   ScreenText.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   4. Februar 2014
 *
 * @brief  Contains the implementation of ScreenText.
 */

#include "ScreenText.h"

#include <boost/assign.hpp>

#include "GLVertexAttributeArray.h"
#include "gfx/Vertices.h"
#include "Font.h"
#include "core/GPUProgramManager.h"

/**
 * Constructor for text from left to right.
 * @param fnt the font to use
 * @param programManager the gpu program manager
 * @param txt the text to render
 * @param pos the position to render at
 * @param fntSize the texts font size in virtual screen pixels
 * @param fntWeight the texts font weight
 * @param fntShearing the texts fonts shearing
 */
ScreenText::ScreenText(const Font& fnt, GPUProgram* fontProg, const std::string& txt,
        const glm::vec2& pos, float fntSize, float fntWeight, float fntShearing, float depth) :
font(fnt),
fontWeight(fntWeight),
fontShearing(fntShearing),
fontSize(fntSize, fntSize),
text(txt),
position(pos),
direction(glm::vec2(1.0f, 0.0f)),
color(1.0f, 1.0f, 1.0f, 1.0f),
depthLayer(depth),
currentBuffer(0),
fontProgram(fontProg),
fontMetricsBindingLocation(nullptr),
pixelLength(0.0f)
{
    Initialize();
}

/**
 * Constructor for text in a certain direction.
 * @param fnt the font to use
 * @param programManager the gpu program manager
 * @param txt the text to render
 * @param pos the position to render at
 * @param dir the direction the text should to
 * @param fntSize the texts font size in virtual screen pixels
 * @param fntWeight the texts font weight
 * @param fntShearing the texts fonts shearing
 */
ScreenText::ScreenText(const Font& fnt, GPUProgram* fontProg, const std::string& txt,
        const glm::vec2& pos, const glm::vec2& dir, float fntSize, float fntWeight,
        float fntShearing, float depth) :
font(fnt),
fontWeight(fntWeight),
fontShearing(fntShearing),
fontSize(fntSize, fntSize),
text(txt),
position(pos),
direction(dir),
color(1.0f, 1.0f, 1.0f, 1.0f),
depthLayer(depth),
currentBuffer(0),
fontProgram(fontProg),
fontMetricsBindingLocation(nullptr),
pixelLength(0.0f)
{
    Initialize();
}

/** Destructor. */
ScreenText::~ScreenText()
{
    for (GLsync sync : textVBOFences) {
        OGL_CALL(glDeleteSync, sync);
    }
    OGL_CALL(glDeleteBuffers, static_cast<GLsizei>(textVBOs.size()), textVBOs.data());
}

/** Initializes the object (workarround for C++98 constructors which can not call other constructors). */
void ScreenText::Initialize()
{
    vertexAttribPos = fontProgram->GetAttributeLocations(
        boost::assign::list_of<std::string>("position")("index"));
    textVBOs.resize(NUM_DYN_BUFFERS, 0);
    textVBOFences.resize(NUM_DYN_BUFFERS, nullptr);
    textVBOSizes.resize(NUM_DYN_BUFFERS, 0);
    OGL_CALL(glGenBuffers, static_cast<GLsizei>(textVBOs.size()), textVBOs.data());
    for (unsigned int i = 0; i < NUM_DYN_BUFFERS; ++i) {
        attribBind.push_back(fontProgram->CreateVertexAttributeArray(textVBOs[i], 0));
    }
    InitializeText(true);
    uniformNames = fontProgram->GetUniformLocations(
        boost::assign::list_of<std::string>("fontStyle")("fontPos")("color")("fontTex"));
    fontMetricsBindingLocation = fontProgram->GetUniformBufferLocation(fontMetricsUBBName);
}

/**
 * Initializes the texts vbo.
 * @param first is this method called from destructor? is so, no need to choose a different buffer
 */
void ScreenText::InitializeText(bool first)
{
    if (!first) {
        currentBuffer = (currentBuffer + 1) % NUM_DYN_BUFFERS;
    }
    std::vector<FontVertex> textVertices(text.size());
    pixelLength = 0.0f;
    for (unsigned int i = 0; i < text.size(); ++i) {
        textVertices[i].idx = font.GetCharacterId(text[i]);
        textVertices[i].pos = glm::vec3(pixelLength * direction, depthLayer);
        pixelLength += font.GetFontMetrics().chars[textVertices[i].idx].xadv * fontSize.x;
    }

    if (textVBOFences[currentBuffer] != nullptr) {
        GLenum result = OGL_CALL(glClientWaitSync, textVBOFences[currentBuffer], 0, ASYNC_TIMEOUT);
        if (result == GL_TIMEOUT_EXPIRED || result == GL_WAIT_FAILED) {
            LOG(ERROR) << L"Waiting for buffer failed.";
            throw std::runtime_error("Waiting for buffer failed.");
        }
#ifdef _DEBUG
        else if (result == GL_CONDITION_SATISFIED) {
            LOG(DEBUG) << L"Waited for buffer ...";
        }
#endif
        OGL_CALL(glDeleteSync, textVBOFences[currentBuffer]);
        textVBOFences[currentBuffer] = nullptr;
    }

    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, textVBOs[currentBuffer]);
    if (textVBOSizes[currentBuffer] < text.size()) {
        OGL_CALL(glBufferData, GL_ARRAY_BUFFER, sizeof (FontVertex) * text.size(),
                nullptr, GL_DYNAMIC_DRAW);
        textVBOSizes[currentBuffer] = static_cast<GLuint>(text.size());

        attribBind[currentBuffer]->StartAttributeSetup();
        if (vertexAttribPos[0]->iBinding >= 0) {
            attribBind[currentBuffer]->AddVertexAttribute(vertexAttribPos[0], 3,
                    GL_FLOAT, GL_FALSE, sizeof (FontVertex), 0);
        }
        if (vertexAttribPos[1]->iBinding >= 0) {
            attribBind[currentBuffer]->AddVertexAttributeI(vertexAttribPos[1], 1,
                    GL_UNSIGNED_INT, sizeof (FontVertex), sizeof (glm::vec3));
        }
        attribBind[currentBuffer]->EndAttributeSetup();
    }

    void* ptr = OGL_CALL(glMapBufferRange, GL_ARRAY_BUFFER, 0, sizeof (FontVertex) * text.size(),
            GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if (ptr == nullptr) {
        throw std::runtime_error("Could not map text vertex buffer.");
    }

    FontVertex* buffer = static_cast<FontVertex*> (ptr);
    std::copy(textVertices.begin(), textVertices.end(), buffer);
    OGL_CALL(glUnmapBuffer, GL_ARRAY_BUFFER);
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
}

/**
 * Returns the length of the text in virual screen pixels.
 * @return the length of the text in virual screen pixels
 */
float ScreenText::GetPixelLength()
{
    return pixelLength;
}

/**
 * Returns the texts baseline
 * @return the baseline
 */
float ScreenText::GetBaseLine()
{
    return font.GetFontMetrics().baseLine * fontSize.y;
}

/**
 * Draws the text.
 * Used for drawing only a single text, so this will set the gpu programs and bind uniform blocks.
 */
void ScreenText::Draw()
{
    font.UseFont(fontProgram, fontMetricsBindingLocation);
    DrawMultiple();
}

/**
 * Draws the text.
 * Used for drawing multiple texts on the screen. GPU programs and ubos will not be set.
 */
void ScreenText::DrawMultiple()
{
    if (textVBOFences[currentBuffer] != nullptr) {
        OGL_CALL(glDeleteSync, textVBOFences[currentBuffer]);
    }

    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, textVBOs[currentBuffer]);
    attribBind[currentBuffer]->EnableVertexAttributeArray();

    glm::vec4 fontStyle(fontWeight, fontShearing * fontSize.y * font.GetFontMetrics().sizeNormalization,
        fontSize.x * font.GetFontMetrics().sizeNormalization,
        fontSize.y * font.GetFontMetrics().sizeNormalization);
    glm::vec4 fontPos(position.x, position.y, direction.x, direction.y);
    fontProgram->SetUniform(uniformNames[0], fontStyle);
    fontProgram->SetUniform(uniformNames[1], fontPos);
    fontProgram->SetUniform(uniformNames[2], color);
    fontProgram->SetUniform(uniformNames[3], 0);
    OGL_CALL(glDrawArrays, GL_POINTS, 0, static_cast<GLsizei>(text.size()));

    attribBind[currentBuffer]->DisableVertexAttributeArray();
    OGL_CALL(glBindBuffer, GL_ARRAY_BUFFER, 0);
    textVBOFences[currentBuffer] = OGL_CALL(glFenceSync, GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

/**
 * Sets a new text to this object.
 * @param txt the text to set
 * @param reinit reinitialize the vertex buffer (use <code>false</code> if you want to make multiple
 * changes and update the buffer on the last change)
 */
void ScreenText::SetText(const std::string& txt, bool reinit)
{
    text = txt;
    if (reinit) InitializeText();
}

/**
 * Sets the texts position on the virtual screen.
 * The y position will be the texts top left corner.
 * @param pos the new position of the text
 */
void ScreenText::SetPosition(const glm::vec2& pos)
{
    position = pos;
}

/**
 * Sets the direction in which the text should go
 * @param dir the new direction
 * @param reinit reinitialize the vertex buffer (use <code>false</code> if you want to make multiple
 * changes and update the buffer on the last change)
 */
void ScreenText::SetDirection(const glm::vec2& dir, bool reinit)
{
    direction = dir;
    if (reinit) InitializeText();
}

/**
 * Sets the text size in virtual screen pixels. Scales the text uniform.
 * @param size the new text size
 * @param reinit reinitialize the vertex buffer (use <code>false</code> if you want to make multiple
 * changes and update the buffer on the last change)
 */
void ScreenText::SetFontSize(float size, bool reinit)
{
    fontSize = glm::vec2(size, size);
    if (reinit) InitializeText();
}

/**
 * Sets the texts size in virtual screen pixels. Scales the text non-uniform.
 * @param size the new non-uniform text size
 * @param reinit reinitialize the vertex buffer (use <code>false</code> if you want to make multiple
 * changes and update the buffer on the last change)
 */
void ScreenText::SetFontSize(const glm::vec2& size, bool reinit)
{
    fontSize = size;
    if (reinit) InitializeText();
}

/**
 * Sets the depth layer the text should be shown on. This is used for the depth buffer test.
 * @param depth the new depth. This value should vary between -1.0f and 1.0f
 * @param reinit reinitialize the vertex buffer (use <code>false</code> if you want to make multiple
 * changes and update the buffer on the last change)
 */
void ScreenText::SetDepthLayer(float depth, bool reinit)
{
    depthLayer = depth;
    if (reinit) InitializeText();
}

/**
 * Sets the fonts weight.
 * @param weight the new font weight.
 */
void ScreenText::SetFontWeight(float weight)
{
    fontWeight = weight;
}

/**
 * Sets the fonts shearing (or italic-ness).
 * @param shearing describes how much the fonts top is shifted towards direction in percent according to font height
 * (1 = 100% means the text will be shifted by exactly the fonts height).
 */
void ScreenText::SetFontShearing(float shearing)
{
    fontShearing = shearing;
}

/**
 * Sets the fonts color.
 * @param col the new color
 */
void ScreenText::SetColor(const glm::vec4& col)
{
    color = col;
}
