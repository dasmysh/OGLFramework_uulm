/**
 * @file   GLTexture2D.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 *
 * @brief  Contains texture class for openGL implementation.
 */

#ifndef GLTEXTURE2D_H
#define GLTEXTURE2D_H

#include "main.h"
#include "core/Resource.h"
#include "GLTexture.h"

/**
 * @brief  2D Texture for the OpenGL implementation.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   31. Dezember 2013
 */
class GLTexture2D : public Resource
{
private:
    /** Copy constructor deleted. */
    GLTexture2D(const GLTexture2D& orig) : Resource(orig) {};
    /** Copy assignment operator deleted. */
    GLTexture2D& operator=(const GLTexture2D&) { return *this; };

public:
    GLTexture2D(const std::string& texFilename, ApplicationBase* app);
    virtual ~GLTexture2D();

    virtual void Load() override;
    virtual void Unload() override;

    GLTexture* GetTexture();

private:
    /** Holds the texture. */
    std::unique_ptr<GLTexture> texture;

    void UnloadLocal();
};

#endif /* GLTEXTURE2D_H */
