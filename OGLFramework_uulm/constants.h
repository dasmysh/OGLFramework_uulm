/**
 * @file   constants.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   5. Februar 2014
 *
 * @brief  Contains global constant definitions.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

/** The configuration file name. */
static const char* configFileName = "patternsConfig.xml";
/** The OpenGL major version used. */
static unsigned int PTRN_OPENGL_MAJOR_VERSION = 3;
/** The OpenGL minor version used. */
static unsigned int PTRN_OPENGL_MINOR_VERSION = 3;

/** Holds the virtual screen size (y) for gui / text rendering. */
static float SCREEN_Y = 1000.0f;

/** The font program resource id. */
static const char* fontProgramID = "renderText.vp|renderText.gp|renderText.fp";
/** The gui program resource id. */
static const char* guiProgramID = "renderGUI.vp|renderGUI.fp";

/** Uniform buffer block name of the font metrics buffer. */
static const char* fontMetricsUBBName = "fontMetrics";
static const char* orthoProjectionUBBName = "orthoProjection";

/** Holds the number of buffers used for dynamic buffering. */
static unsigned int NUM_DYN_BUFFERS = 5;
/** Holds the timeout to wait for asynchronus buffers. */
static GLuint64 ASYNC_TIMEOUT = 3000000;

#endif /* CONSTANTS_H */
