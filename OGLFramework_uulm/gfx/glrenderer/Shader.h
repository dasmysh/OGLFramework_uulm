/**
 * @file   Shader.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the definition of Shader.
 */

#ifndef SHADER_H
#define SHADER_H

#include "main.h"

namespace cgu {

    class GPUProgram;

    using compiler_error_info = boost::error_info<struct tag_compiler_error, std::string>;
    using fileid_info = boost::error_info<struct tag_fileid, unsigned int>;
    using lineno_info = boost::error_info<struct tag_lineno, unsigned int>;

    /**
     * Exception class for shader compiler errors.
     */
    struct shader_compiler_error : virtual resource_loading_error { };
    /*class shader_compiler_error : public virtual resource_loading_error
    {
    public:
        shader_compiler_error() : resource_loading_error() {}
        shader_compiler_error(const shader_compiler_error&) = default;
        shader_compiler_error operator=(const shader_compiler_error&) = delete;
        shader_compiler_error(shader_compiler_error&& rhs) : resource_loading_error(std::move(rhs)) {}
        shader_compiler_error operator=(shader_compiler_error&&) = delete;
        virtual ~shader_compiler_error() = default;
    };*/

    /*class shader_compiler_error : public std::exception
    {
    public:
        shader_compiler_error(const std::string& shader, const std::string& errors);
        shader_compiler_error(const shader_compiler_error&);
        shader_compiler_error& operator=(const shader_compiler_error&);
        shader_compiler_error(shader_compiler_error&&);
        shader_compiler_error& operator=(shader_compiler_error&&);
        virtual const char* what() const override;

    private:/*
        /** Holds the shader name. */
        //std::unique_ptr<char[]> shr;
        /** Holds the errors. */
        //std::unique_ptr<char[]> errs;
        /** holds the error message. */
        //std::unique_ptr<char[]> myWhat;
    //};

    /**
     * @brief  The resource type for shaders.
     *
     * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
     * @date   2014.01.15
     */
    class Shader : public Resource
    {
    public:
        Shader(const std::string& shaderFilename, ApplicationBase* app);
        Shader(const Shader&);
        Shader& operator=(const Shader&);
        Shader(Shader&&);
        Shader& operator=(Shader&&);
        virtual ~Shader();

        void Load() override;
        void Unload() override;
        void ResetShader(GLuint newShader);

        GLuint RecompileShader() const;

    private:
        friend GPUProgram;

        /** Holds the compiled shader. */
        GLuint shader;
        /** Holds the shaders type. */
        GLenum type;
        /** Holds the shaders type as a string. */
        std::string strType;

        void UnloadLocal();
        GLuint CompileShader(const std::string& filename, const std::vector<std::string>& defines, GLenum type, const std::string& strType) const;
        std::string LoadShaderFile(const std::string& filename, const std::vector<std::string>& defines, unsigned int& fileId, unsigned int recursionDepth) const;
    };
}

#endif /* SHADER_H */
