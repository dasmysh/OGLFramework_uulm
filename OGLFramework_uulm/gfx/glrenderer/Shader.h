/**
 * @file   Shader.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the definition of Shader.
 */

#ifndef SHADER_H
#define SHADER_H

#include "main.h"

namespace cgu {
    class GPUProgram;

    /**
     * Exception class for shader compiler errors.
     */
    class shader_compiler_error : public std::exception
    {
    public:
        shader_compiler_error(const std::string& shader, const std::string& errors);
        shader_compiler_error(const shader_compiler_error&);
        shader_compiler_error& operator=(const shader_compiler_error&);
        shader_compiler_error(shader_compiler_error&&);
        shader_compiler_error& operator=(shader_compiler_error&&);
        virtual const char* what() const;

    private:
        /** Holds the shader name. */
        std::unique_ptr<char[]> shr;
        /** Holds the errors. */
        std::unique_ptr<char[]> errs;
        /** holds the error message. */
        std::unique_ptr<char[]> myWhat;
    };

    /**
     * @brief  The resource type for shaders.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   15. Januar 2014
     */
    class Shader : public Resource
    {
    private:
        /** Deleted copy constructor. */
        Shader(const Shader& orig) : Resource(orig) {};
        /** Deleted copy assignment operator. */
        Shader& operator=(const Shader&) {};

    public:
        Shader(const std::string& shaderFilename, ApplicationBase* app);
        virtual ~Shader();
        /** Move constructor. */
        Shader(Shader&& orig);
        /** Move assignment operator. */
        Shader& operator=(Shader&& orig);

        virtual void Load() override;
        virtual void Unload() override;
        void ResetShader(GLuint newShader);

        GLuint RecompileShader();

    private:
        friend GPUProgram;

        /** Holds the compiled shader. */
        GLuint shader;
        /** Holds the shaders type. */
        GLenum type;
        /** Holds the shaders type as a string. */
        std::string strType;

        void UnloadLocal();
        static GLuint CompileShader(const std::string& filename, const std::vector<std::string>& defines, GLenum type, const std::string& strType);
        static std::string LoadShaderFile(const std::string& filename, const std::vector<std::string>& defines, unsigned int& fileId, unsigned int recursionDepth);
    };
}

#endif /* SHADER_H */
