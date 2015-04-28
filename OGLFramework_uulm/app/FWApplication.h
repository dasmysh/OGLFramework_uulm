/**
 * @file   FWApplication.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Januar 2014
 *
 * @brief  Contains the definition of FWApplication.
 */

#ifndef FWAPPLICATION_H
#define FWAPPLICATION_H

#include "ApplicationBase.h"
#include "gfx/glrenderer/MeshRenderable.h"
#include "gfx/glrenderer/GPUProgram.h"
#include "gfx/glrenderer/GLUniformBuffer.h"
#include "gfx/glrenderer/Font.h"
#include "gfx/glrenderer/ScreenText.h"
#include "gfx/OBJMesh.h"
// TODO: no dialogs today ...
// #include "gui/DialogManager.h"

/**
 * @brief Implementation of the framework application.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   21. Januar 2014
 */
class FWApplication : public ApplicationBase
{
public:
    FWApplication(GLWindow& window);
    virtual ~FWApplication();

    virtual void HandleKeyDown(unsigned int keyCode);

private:
    /** Holds the dialog manager. */
    // std::unique_ptr<DialogManager> dialogManager = {};
    /** Holds the shader used. */
    GPUProgram* program;
    /** Holds the vertex attribute bindings. */
    VertexAttributeBindings bind;
    /** Holds the object mesh used. */
    std::unique_ptr<OBJMesh> objMesh;
    /** Holds the rendered mesh. */
    std::unique_ptr<MeshRenderable> mesh;
    /** Holds a uniform buffer containing a color. */
    std::unique_ptr<GLUniformBuffer> uBuffer;

    /** Holds the shader program used for font rendering. */
    GPUProgram* fontProgram;
    /** Holds the font to use. */
    std::unique_ptr<Font> font;
    /** Holds the screen text to render. */
    std::unique_ptr<ScreenText> text;

    /** holds the gui renderable for a textured quad. */
    std::unique_ptr<GUIRenderable> quad;
    /** holds the quads texture. */
    std::unique_ptr<GLTexture> quadTex;

protected:
    void FrameMove(float time, float elapsed);
    void RenderScene();
};

#endif /* FWAPPLICATION_H */
