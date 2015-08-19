/**
 * @file   GPUProgram.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   15. Januar 2014
 *
 * @brief  Contains the implementation of GPUProgram.
 */

#include "GPUProgram.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include "ShaderBufferBindingPoints.h"
#include "app/ApplicationBase.h"
#include "GLTexture.h"
#include "GLUniformBuffer.h"

namespace cgu {

    /**
     * Constructor.
     * @param programName the programs name
     * @param win the base window this manager belongs to holding all needed dependencies.
     */
    GPUProgram::GPUProgram(const std::string programName, ApplicationBase* app) :
        Resource(programName, app),
        program(0),
        knownVABindings(),
        knownUniformBindings(),
        knownUBBindings(),
        boundUBlocks(),
        knownSSBOBindings(),
        boundSSBOs(),
        vaos()
    {

    }

    /** Destructor. */
    GPUProgram::~GPUProgram()
    {
        if (IsLoaded()) UnloadLocal();
    }

    /**
     * Move constructor.
     * @param orig the original object
     */
    GPUProgram::GPUProgram(GPUProgram&& orig) :
        Resource(std::move(orig)),
        program(orig.program)
    {
        orig.program = 0;
    }

    /**
     * Move assignment operator.
     * @param orig the original object
     * @return reference to this object
     */
    GPUProgram& GPUProgram::operator =(GPUProgram&& orig)
    {
        Resource* tRes = this;
        *tRes = static_cast<Resource&&> (std::move(orig));
        if (this != &orig) {
            program = orig.program;
            orig.program = 0;
        }
        return *this;
    }

    void GPUProgram::Load()
    {
        std::vector<std::string> programNames;
        boost::split(programNames, id, boost::is_any_of("|"));
        std::vector<GLuint> shaders;
        for (auto& progName : programNames) {
            boost::trim(progName);
            // ignore exception and reload whole program
            shaders.push_back(application->GetShaderManager()->GetResource(progName)->shader);
        }
        LoadInternal(LinkNewProgram(id, shaders));
    }

    /**
     * Internal load function to be called after the program has been initialized.
     * @param newProgram the new initialized program to set
     */
    void GPUProgram::LoadInternal(GLuint newProgram)
    {
        this->program = newProgram;

        for (auto& ab : knownVABindings) {
            ab.second->iBinding = OGL_CALL(glGetAttribLocation, this->program, ab.first.c_str());
        }

        for (auto& ub : knownUniformBindings) {
            ub.second->iBinding = OGL_CALL(glGetUniformLocation, this->program, ub.first.c_str());
        }

        for (auto& ubb : knownUBBindings) {
            ubb.second->uBinding = OGL_CALL(glGetUniformBlockIndex, this->program, ubb.first.c_str());
        }

        for (auto& bublock : boundUBlocks) {
            BindUniformBlock(bublock.first, bublock.second);
        }

        for (auto& ssbo : knownSSBOBindings) {
            ssbo.second->uBinding = OGL_CALL(glGetProgramResourceIndex, this->program, GL_SHADER_STORAGE_BLOCK, ssbo.first.c_str());
        }

        for (auto& ssbo : boundSSBOs) {
            BindShaderBuffer(ssbo.first, ssbo.second);
        }

        for (auto& vao : vaos) {
            vao->DisableAttributes();
            vao->UpdateVertexAttributes();
        }
        Resource::Load();
    }

    /** Recompiles the program. */
    void GPUProgram::RecompileProgram()
    {
        std::vector<std::string> programNames;
        boost::split(programNames, id, boost::is_any_of("|"));
        std::vector<Shader*> shaders;
        std::vector<GLuint> newOGLShaders;
        for (auto& progName : programNames) {
            boost::trim(progName);
            shaders.push_back(application->GetShaderManager()->GetResource(progName));
        }
        newOGLShaders.resize(shaders.size(), 0);
        for (unsigned int i = 0; i < shaders.size(); ++i) {
            while (newOGLShaders[i] == 0) {
                try {
                    newOGLShaders[i] = shaders[i]->RecompileShader();
                }
                catch (shader_compiler_error compilerError) {
                    ReleaseShaders(newOGLShaders);
                    throw compilerError;
                }
            }
        }

        GLuint tempProgram = 0;
        try {
            tempProgram = LinkNewProgram(id, newOGLShaders);
        }
        catch (shader_compiler_error compilerError) {
            ReleaseShaders(newOGLShaders);
            throw compilerError;
        }

        Unload();
        for (unsigned int i = 0; i < shaders.size(); ++i) {
            shaders[i]->ResetShader(newOGLShaders[i]);
        }
        LoadInternal(tempProgram);
    }

    /**
     * Get the vertex attribute locations in this program.
     * @param attribNames the names of the attributes to find
     * @return the list of locations
     */
    std::vector<BindingLocation> GPUProgram::GetAttributeLocations(const std::vector<std::string>& attribNames)
    {
        std::vector<BindingLocation> result;
        result.reserve(attribNames.size());
        for (const std::string& name : attribNames) {
            try {
                result.push_back(knownVABindings.at(name).get());
            }
            catch (std::out_of_range e) {
                BindingLocationInternal binding(new shader_binding_desc());
                binding->iBinding = OGL_CALL(glGetAttribLocation, this->program, name.c_str());
                result.push_back(knownVABindings.insert(std::make_pair(name, std::move(binding))).first->second.get());
            }
        }
        return result;
    }

    /**
     * Creates a new vertex attribute array.
     * @param vBuffer the vertex buffer the array is for
     * @param iBuffer the index buffer used
     * @return pointer to the new vertex attribute array
     */
    GLVertexAttributeArray* GPUProgram::CreateVertexAttributeArray(GLuint vBuffer, GLuint iBuffer)
    {
        vaos.push_back(std::unique_ptr<GLVertexAttributeArray>(new GLVertexAttributeArray(vBuffer, iBuffer)));
        return vaos.back().get();
    }

    /**
     * Get an uniform location of this program.
     * @param uniformName the name of the uniform
     * @return the uniforms location
     */
    BindingLocation GPUProgram::GetUniformLocation(const std::string& uniformName)
    {
        try {
            return knownUniformBindings.at(uniformName).get();
        }
        catch (std::out_of_range e) {
            BindingLocationInternal binding(new shader_binding_desc());
            binding->iBinding = OGL_CALL(glGetUniformLocation, this->program, uniformName.c_str());
            return knownUniformBindings.insert(std::make_pair(uniformName, std::move(binding))).first->second.get();
        }
    }

    /**
     * Get locations of the uniforms by their names.
     * @param uniformNames the uniform names
     * @return a vector of the locations
     */
    std::vector<BindingLocation> GPUProgram::GetUniformLocations(const std::vector<std::string>& uniformNames)
    {
        std::vector<BindingLocation> result;
        result.reserve(uniformNames.size());
        for (const std::string& name : uniformNames) {
            try {
                result.push_back(knownUniformBindings.at(name).get());
            }
            catch (std::out_of_range e) {
                BindingLocationInternal binding(new shader_binding_desc());
                binding->iBinding = OGL_CALL(glGetUniformLocation, this->program, name.c_str());
                result.push_back(knownUniformBindings.insert(std::make_pair(name, std::move(binding))).first->second.get());
            }
        }
        return result;
    }

    /**
     * Returns the opengl location of the ubo with given name.
     * @param uBufferName the uniform buffer name
     * @return the ubo location
     */
    BindingLocation GPUProgram::GetUniformBufferLocation(const std::string& uBufferName)
    {
        try {
            return knownUBBindings.at(uBufferName).get();
        }
        catch (std::out_of_range e) {
            BindingLocationInternal binding(new shader_binding_desc());
            binding->uBinding = OGL_CALL(glGetUniformBlockIndex, this->program, uBufferName.c_str());
            if (binding->uBinding == GL_INVALID_INDEX)
                throw std::out_of_range("Could not find uniform buffer \"" + uBufferName + "\".");
            return knownUBBindings.insert(std::make_pair(uBufferName, std::move(binding))).first->second.get();
        }
    }

    /**
     * Sets a uniform with given opengl name/location (vec4 version)
     * @param name the location of the uniform
     * @param data the vec4 to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const glm::vec4& data)
    {
        OGL_CALL(glUniform4fv, name->iBinding, 1, reinterpret_cast<const GLfloat*> (&data));
    }

    /**
     * Sets a uniform with given opengl name/location (float[] version)
     * @param name the location of the uniform
     * @param data the float[] to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const std::vector<float>& data)
    {
        OGL_CALL(glUniform1fv, name->iBinding, static_cast<GLsizei>(data.size()), data.data());
    }

    /**
     * Sets a uniform with given opengl name/location (int version)
     * @param name the location of the uniform
     * @param data the int to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, int data)
    {
        OGL_CALL(glUniform1i, name->iBinding, data);
    }

    /**
    * Sets a uniform with given opengl name/location (int version)
    * @param name the location of the uniform
    * @param data the uint vector to set the uniform to
    */
    void GPUProgram::SetUniform(BindingLocation name, const glm::uvec3& data)
    {
        OGL_CALL(glUniform3ui, name->iBinding, data.x, data.y, data.z);
    }

    /**
     * Bind a uniform block with given name to a ubo binding point
     * @param uBufferName the ubo name
     * @param bindingPoints the binding points
     */
    void GPUProgram::BindUniformBlock(const std::string& uBufferName, ShaderBufferBindingPoints& bindingPoints)
    {
        GLuint bindingPoint = bindingPoints.GetBindingPoint(uBufferName);
        BindUniformBlock(uBufferName, bindingPoint);
    }

    /**
     * Bind a uniform block with given name to a ubo binding point
     * @param uBufferName the ubo name
     * @param bindingPoint the binding point
     */
    void GPUProgram::BindUniformBlock(const std::string& uBufferName, GLuint bindingPoint)
    {
        BindingLocation uBufferLoc = GetUniformBufferLocation(uBufferName);
        BindUniformBlock(uBufferLoc, bindingPoint);
        boundUBlocks[uBufferName] = bindingPoint;
    }

    /**
     * Bind a uniform block with given name to a ubo binding point
     * @param name the ubo name
     * @param bindingPoint the binding point
     */
    void GPUProgram::BindUniformBlock(BindingLocation name, GLuint bindingPoint)
    {
        OGL_CALL(glUniformBlockBinding, this->program, name->uBinding, bindingPoint);
    }

    /**
    * Returns the opengl location of the ssbo with given name.
    * @param sBufferName the uniform buffer name
    * @return the ssbo location
    */
    BindingLocation GPUProgram::GetShaderBufferLocation(const std::string& sBufferName)
    {
        try {
            return knownSSBOBindings.at(sBufferName).get();
        }
        catch (std::out_of_range e) {
            BindingLocationInternal binding(new shader_binding_desc());
            binding->uBinding = OGL_CALL(glGetProgramResourceIndex, this->program, GL_SHADER_STORAGE_BLOCK, sBufferName.c_str());
            if (binding->uBinding == GL_INVALID_INDEX)
                throw std::out_of_range("Could not find uniform buffer \"" + sBufferName + "\".");
            return knownSSBOBindings.insert(std::make_pair(sBufferName, std::move(binding))).first->second.get();
        }
    }

    /**
    * Bind a ssbo with given name to a ssbo binding point
    * @param sBufferName the ssbo name
    * @param bindingPoints the binding points
    */
    void GPUProgram::BindShaderBuffer(const std::string& sBufferName, ShaderBufferBindingPoints& bindingPoints)
    {
        GLuint bindingPoint = bindingPoints.GetBindingPoint(sBufferName);
        BindShaderBuffer(sBufferName, bindingPoint);
    }

    /**
    * Bind a ssbo with given name to a ssbo binding point
    * @param sBufferName the ssbo name
    * @param bindingPoint the binding point
    */
    void GPUProgram::BindShaderBuffer(const std::string& sBufferName, GLuint bindingPoint)
    {
        BindingLocation sBufferLoc = GetShaderBufferLocation(sBufferName);
        BindShaderBuffer(sBufferLoc, bindingPoint);
        boundSSBOs[sBufferName] = bindingPoint;
    }

    /**
    * Bind a ssbo with given name to a ssbo binding point
    * @param name the ssbo name
    * @param bindingPoint the binding point
    */
    void GPUProgram::BindShaderBuffer(BindingLocation name, GLuint bindingPoint)
    {
        OGL_CALL(glShaderStorageBlockBinding, this->program, name->uBinding, bindingPoint);
    }

    /**
     * Activate this program for rendering use.
     */
    void GPUProgram::UseProgram() const
    {
        OGL_CALL(glUseProgram, this->program);
    }

    void GPUProgram::UnloadLocal()
    {
        if (this->program != 0) {
            OGL_CALL(glDeleteProgram, this->program);
            this->program = 0;
        }
    }

    void GPUProgram::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }

    GLuint GPUProgram::LinkNewProgram(const std::string& name, const std::vector<GLuint>& shaders)
    {
        GLuint program = OGL_SCALL(glCreateProgram);
        if (program == 0) {
            LOG(ERROR) << L"Could not create gpu program!";
            throw std::runtime_error("Could not create gpu program!");
        }
        for (auto shader : shaders) {
            OGL_CALL(glAttachShader, program, shader);
        }
        OGL_CALL(glLinkProgram, program);

        GLint status;
        OGL_CALL(glGetProgramiv, program, GL_LINK_STATUS, &status);
        if (status == GL_FALSE) {
            GLint infoLogLength;
            OGL_CALL(glGetProgramiv, program, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar *strInfoLog = new GLchar[infoLogLength + 1];
            OGL_CALL(glGetProgramInfoLog, program, infoLogLength, NULL, strInfoLog);
            LOG(ERROR) << L"Linker failure: " << strInfoLog;
            std::string infoLog = strInfoLog;
            delete[] strInfoLog;

            for (auto shader : shaders) {
                OGL_CALL(glDetachShader, program, shader);
            }
            OGL_CALL(glDeleteProgram, program);

            throw shader_compiler_error(name, infoLog);
        }
        for (auto shader : shaders) {
            OGL_CALL(glDetachShader, program, shader);
        }
        return program;
    }

    void GPUProgram::ReleaseShaders(const std::vector<GLuint>& shaders)
    {
        for (auto shader : shaders) {
            if (shader != 0) {
                OGL_CALL(glDeleteShader, shader);
            }
        }
    }
}
