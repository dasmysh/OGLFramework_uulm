/**
 * @file   GPUProgram.h
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the definition of GPUProgram.
 */

#ifndef GPUPROGRAM_H
#define GPUPROGRAM_H

#include "main.h"
#include "core/ShaderManager.h"
#include "GLVertexAttributeArray.h"

class ShaderBufferBindingPoints;
class GLRenderTarget;
class ApplicationBase;

/**
 * @brief  Complete gpu program with multiple Shader objects working together.
 *
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 */
class GPUProgram : public Resource
{
private:
    /** Deleted copy constructor. */
    GPUProgram(const GPUProgram& orig) : Resource(orig) {};
    /** Deleted copy assignment operator. */
    GPUProgram& operator=(const GPUProgram&) { return *this; };

public:
    GPUProgram(const std::string programName, ApplicationBase* app);
    virtual ~GPUProgram();
    GPUProgram(GPUProgram&& orig);
    GPUProgram& operator=(GPUProgram&& orig);

    void Load() override final;
    void Unload() override final;

    void RecompileProgram();

    GLVertexAttributeArray* CreateVertexAttributeArray(GLuint vBuffer, GLuint iBuffer);
    std::vector<BindingLocation> GetAttributeLocations(const std::vector<std::string>& attribNames);
    BindingLocation GetUniformLocation(const std::string& uniformName);
    std::vector<BindingLocation> GetUniformLocations(const std::vector<std::string>& uniformNames);
    void SetUniform(BindingLocation name, const glm::vec4& data);
    void SetUniform(BindingLocation name, const std::vector<float>& data);
    void SetUniform(BindingLocation name, int data);
    BindingLocation GetUniformBufferLocation(const std::string& uBufferName);
    void BindUniformBlock(const std::string& uBufferName, ShaderBufferBindingPoints& bindingPoints);
    void BindUniformBlock(const std::string& uBufferName, GLuint bindingPoint);
    void BindUniformBlock(BindingLocation name, GLuint bindingPoint);
    void UseProgram() const;

private:
    friend GLRenderTarget;
    typedef std::unique_ptr<shader_binding_desc> BindingLocationInternal;
    /** Holds the program. */
    GLuint program;
    /** holds the known vertex attribute bindings. */
    std::unordered_map<std::string, BindingLocationInternal> knownVABindings;
    /** holds the known uniform locations. */
    std::unordered_map<std::string, BindingLocationInternal> knownUniformBindings;
    /** holds the known uniform buffer locations. */
    std::unordered_map<std::string, BindingLocationInternal> knownUBBindings;
    /** holds the bound uniform blocks. */
    std::unordered_map<std::string, GLuint> boundUBlocks;
    /** holds the vertex attribute arrays associated with this gpu program. */
    std::vector<std::unique_ptr<GLVertexAttributeArray> > vaos;

    void UnloadLocal();
    void LoadInternal(GLuint newProgram);
    static GLuint LinkNewProgram(const std::string& name, const std::vector<GLuint>& shaders);
    static void ReleaseShaders(const std::vector<GLuint>& shaders);
};

#endif /* GPUPROGRAM_H */
