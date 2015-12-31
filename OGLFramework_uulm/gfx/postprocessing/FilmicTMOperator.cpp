/**
 * @file   FilmicTMOperator.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.12.21
 *
 * @brief  Implementation of the filmic tone-mapping operator.
 */

#include "FilmicTMOperator.h"
#include "app/ApplicationBase.h"
#include "gfx/glrenderer/GLRenderTarget.h"
#include "gfx/glrenderer/GLUniformBuffer.h"
#include "gfx/glrenderer/ScreenQuadRenderable.h"

namespace cgu {

    FilmicTMOperator::FilmicTMOperator(ApplicationBase* app) :
        tmProgram(app->GetGPUProgramManager()->GetResource("shader/screenQuad.vp|shader/tm/filmic.fp")),
        renderable(app->GetScreenQuadRenderable()),
        uniformIds(tmProgram->GetUniformLocations({ "sourceTex" })),
        filmicUBO(new GLUniformBuffer("filmicBuffer", sizeof(FilmicTMParameters), app->GetUBOBindingPoints()))
    {
        params.sStrength = 0.15f;
        params.linStrength = 0.5f;
        params.linAngle = 0.1f;
        params.toeStrength = 0.2f;
        params.toeNumerator = 0.02f;
        params.toeDenominator = 0.3f;
        params.white = 11.2f;
        params.gamma = 2.2f;

        tmProgram->BindUniformBlock("filmicBuffer", *app->GetUBOBindingPoints());

        // Alternative values:
        /*params.sStrength = 0.22f;
        params.linStrength = 0.3f;
        params.linAngle = 0.1f;
        params.toeStrength = 0.2f;
        params.toeNumerator = 0.1f;
        params.toeDenominator = 0.3f;
        params.white = 11.2f;
        params.gamma = 2.2f;*/
    }

    /** Default destructor. */
    FilmicTMOperator::~FilmicTMOperator() = default;

    void FilmicTMOperator::AddParameterSlidersToBar(TwBar* bar)
    {
        TwAddVarRW(bar, "FTMSStr", TW_TYPE_FLOAT, &params.sStrength, " label='Shoulder Strength' min=0.1 max=1 step=0.01");
        TwAddVarRW(bar, "FTMLinStr", TW_TYPE_FLOAT, &params.linStrength, " label='Linear Strength' min=0.1 max=1 step=0.1");
        TwAddVarRW(bar, "FTMLinAngle", TW_TYPE_FLOAT, &params.linAngle, " label='Linear Angle' min=0.01 max=1 step=0.01");
        TwAddVarRW(bar, "FTMToeStr", TW_TYPE_FLOAT, &params.toeStrength, " label='Toe Strength' min=0.1 max=1 step=0.1");
        TwAddVarRW(bar, "FTMToeNum", TW_TYPE_FLOAT, &params.toeNumerator, " label='Toe Numerator' min=0.01 max=0.1 step=0.01");
        TwAddVarRW(bar, "FTMToeDenom", TW_TYPE_FLOAT, &params.toeDenominator, " label='Toe Denominator' min=0.1 max=1 step=0.1");
        TwAddVarRW(bar, "FTMWhite", TW_TYPE_FLOAT, &params.white, " label='White' min=5.0 max=50 step=0.1");
        TwAddVarRW(bar, "Gamma", TW_TYPE_FLOAT, &params.gamma, " label='Gamma' min=1.0 max=3.0 step=0.1");
    }

    void FilmicTMOperator::ApplyTonemapping(GLRenderTarget* sourceRT, GLRenderTarget* targetRT)
    {
        filmicUBO->UploadData(0, sizeof(FilmicTMParameters), &params);
        filmicUBO->BindBuffer();

        targetRT->BatchDraw([this, sourceRT](cgu::GLBatchRenderTarget & brt) {
            tmProgram->UseProgram();
            sourceRT->GetTextures()[0]->ActivateTexture(GL_TEXTURE0);
            tmProgram->SetUniform(uniformIds[0], 0);
            renderable->Draw();
        });
    }
}
