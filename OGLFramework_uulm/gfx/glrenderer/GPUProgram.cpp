/**
 * @file   GPUProgram.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.15
 *
 * @brief  Contains the implementation of GPUProgram.
 */

#include "GPUProgram.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include "ShaderBufferBindingPoints.h"
#include "app/ApplicationBase.h"

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

    /** Copy constructor. */
    GPUProgram::GPUProgram(const GPUProgram& rhs) : GPUProgram(rhs.id, rhs.application)
    {
        if (rhs.IsLoaded()) GPUProgram::Load();
    }

    /** Copy assignment operator. */
    GPUProgram& GPUProgram::operator=(const GPUProgram& rhs)
    {
        Resource* tRes = this;
        *tRes = static_cast<const Resource&>(rhs);
        GPUProgram tmp{ rhs };
        std::swap(program, tmp.program);
        std::swap(knownVABindings, tmp.knownVABindings);
        std::swap(knownUniformBindings, tmp.knownUniformBindings);
        std::swap(knownUBBindings, tmp.knownUBBindings);
        std::swap(boundUBlocks, tmp.boundUBlocks);
        std::swap(knownSSBOBindings, tmp.knownSSBOBindings);
        std::swap(boundSSBOs, tmp.boundSSBOs);
        std::swap(vaos, tmp.vaos);
        return *this;
    }

    /**
     * Move constructor.
     * @param rhs the original object
     */
    GPUProgram::GPUProgram(GPUProgram&& rhs) :
        Resource(std::move(rhs)),
        program(rhs.program),
        knownVABindings(std::move(rhs.knownVABindings)),
        knownUniformBindings(std::move(rhs.knownUniformBindings)),
        knownUBBindings(std::move(rhs.knownUBBindings)),
        boundUBlocks(std::move(rhs.boundUBlocks)),
        knownSSBOBindings(std::move(rhs.knownSSBOBindings)),
        boundSSBOs(std::move(rhs.boundSSBOs)),
        vaos(std::move(rhs.vaos))
    {
        rhs.program = 0;
    }

    /**
     * Move assignment operator.
     * @param rhs the original object
     * @return reference to this object
     */
    GPUProgram& GPUProgram::operator =(GPUProgram&& rhs)
    {
        Resource* tRes = this;
        *tRes = static_cast<Resource&&> (std::move(rhs));
        if (this != &rhs) {
            program = rhs.program;
            rhs.program = 0;
            knownVABindings = std::move(rhs.knownVABindings);
            knownUniformBindings = std::move(rhs.knownUniformBindings);
            knownUBBindings = std::move(rhs.knownUBBindings);
            boundUBlocks = std::move(rhs.boundUBlocks);
            knownSSBOBindings = std::move(rhs.knownSSBOBindings);
            boundSSBOs = std::move(rhs.boundSSBOs);
            vaos = std::move(rhs.vaos);
        }
        return *this;
    }

    void GPUProgram::Load()
    {
        auto programNames = GetSubresources();
        std::vector<GLuint> shaders;
        for (auto& progName : programNames) {
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
        std::vector<std::string> progDefinition;
        boost::split(progDefinition, id, boost::is_any_of(","));
        std::vector<std::string> programNames;
        boost::split(programNames, progDefinition[0], boost::is_any_of("|"));
        std::vector<Shader*> shaders;
        std::vector<GLuint> newOGLShaders;
        for (auto& progName : programNames) {
            boost::trim(progName);
            for (unsigned int i = 1; i < progDefinition.size(); ++i) progName += "," + progDefinition[i];
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
                    throw;
                }
            }
        }

        GLuint tempProgram = 0;
        try {
            tempProgram = LinkNewProgram(id, newOGLShaders);
        }
        catch (shader_compiler_error compilerError) {
            ReleaseShaders(newOGLShaders);
            throw;
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
        vaos.push_back(std::make_unique<GLVertexAttributeArray>(vBuffer, iBuffer));
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
     * Returns the OpenGL location of the UBO with given name.
     * @param uBufferName the uniform buffer name
     * @return the UBO location
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
     * Sets a uniform with given OpenGL name/location (vec2 version)
     * @param name the location of the uniform
     * @param data the vec2 to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const glm::vec2& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform2fv, name->iBinding, 1, reinterpret_cast<const GLfloat*> (&data));
    }

    /**
     * Sets a uniform with given OpenGL name/location (vec3 version)
     * @param name the location of the uniform
     * @param data the vec3 to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const glm::vec3& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform3fv, name->iBinding, 1, reinterpret_cast<const GLfloat*> (&data));
    }

    /**
     * Sets a uniform with given OpenGL name/location (vec4 version)
     * @param name the location of the uniform
     * @param data the vec4 to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const glm::vec4& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform4fv, name->iBinding, 1, reinterpret_cast<const GLfloat*> (&data));
    }

    /**
     * Sets a uniform with given OpenGL name/location (float[] version)
     * @param name the location of the uniform
     * @param data the float[] to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const std::vector<float>& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform1fv, name->iBinding, static_cast<GLsizei>(data.size()), data.data());
    }

    /**
     * Sets a uniform with given OpenGL name/location (int[] version)
     * @param name the location of the uniform
     * @param data the int[] to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, const std::vector<int>& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform1iv, name->iBinding, static_cast<GLsizei>(data.size()), data.data());
    }

    /**
     * Sets a uniform with given OpenGL name/location (int version)
     * @param name the location of the uniform
     * @param data the int to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, int data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform1i, name->iBinding, data);
    }

    /**
     * Sets a uniform with given OpenGL name/location (float version)
     * @param name the location of the uniform
     * @param data the float to set the uniform to
     */
    void GPUProgram::SetUniform(BindingLocation name, float data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform1f, name->iBinding, data);
    }

    /**
    * Sets a uniform with given OpenGL name/location (int version)
    * @param name the location of the uniform
    * @param data the uint vector to set the uniform to
    */
    void GPUProgram::SetUniform(BindingLocation name, const glm::uvec3& data) const
    {
        GLint cProg;
        OGL_CALL(glGetIntegerv, GL_CURRENT_PROGRAM, &cProg);
        assert(program == cProg);
        OGL_CALL(glUniform3ui, name->iBinding, data.x, data.y, data.z);
    }

    /**
     * Bind a uniform block with given name to a UBO binding point
     * @param uBufferName the UBO name
     * @param bindingPoints the binding points
     */
    void GPUProgram::BindUniformBlock(const std::string& uBufferName, ShaderBufferBindingPoints& bindingPoints)
    {
        auto bindingPoint = bindingPoints.GetBindingPoint(uBufferName);
        BindUniformBlock(uBufferName, bindingPoint);
    }

    /**
     * Bind a uniform block with given name to a UBO binding point
     * @param uBufferName the UBO name
     * @param bindingPoint the binding point
     */
    void GPUProgram::BindUniformBlock(const std::string& uBufferName, GLuint bindingPoint)
    {
        BindingLocation uBufferLoc = GetUniformBufferLocation(uBufferName);
        BindUniformBlock(uBufferLoc, bindingPoint);
        boundUBlocks[uBufferName] = bindingPoint;
    }

    /**
     * Bind a uniform block with given name to a UBO binding point
     * @param name the UBO name
     * @param bindingPoint the binding point
     */
    void GPUProgram::BindUniformBlock(BindingLocation name, GLuint bindingPoint) const
    {
        OGL_CALL(glUniformBlockBinding, this->program, name->uBinding, bindingPoint);
    }

    /**
    * Returns the OpenGL location of the SSBO with given name.
    * @param sBufferName the uniform buffer name
    * @return the SSBO location
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
    * Bind a SSBO with given name to a SSBO binding point
    * @param sBufferName the SSBO name
    * @param bindingPoints the binding points
    */
    void GPUProgram::BindShaderBuffer(const std::string& sBufferName, ShaderBufferBindingPoints& bindingPoints)
    {
        GLuint bindingPoint = bindingPoints.GetBindingPoint(sBufferName);
        BindShaderBuffer(sBufferName, bindingPoint);
    }

    /**
    * Bind a SSBO with given name to a SSBO binding point
    * @param sBufferName the SSBO name
    * @param bindingPoint the binding point
    */
    void GPUProgram::BindShaderBuffer(const std::string& sBufferName, GLuint bindingPoint)
    {
        BindingLocation sBufferLoc = GetShaderBufferLocation(sBufferName);
        BindShaderBuffer(sBufferLoc, bindingPoint);
        boundSSBOs[sBufferName] = bindingPoint;
    }

    /**
    * Bind a SSBO with given name to a SSBO binding point
    * @param name the SSBO name
    * @param bindingPoint the binding point
    */
    void GPUProgram::BindShaderBuffer(BindingLocation name, GLuint bindingPoint) const
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

    GLuint GPUProgram::LinkNewProgram(const std::string& name, const std::vector<GLuint>& shaders) const
    {
        GLuint program = OGL_SCALL(glCreateProgram);
        if (program == 0) {
            LOG(ERROR) << L"Could not create GPU program!";
            throw resource_loading_error() << resid_info(name)
                << errdesc_info("Cannot create program.");
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

            auto strInfoLog = new GLchar[infoLogLength + 1];
            OGL_CALL(glGetProgramInfoLog, program, infoLogLength, NULL, strInfoLog);
            LOG(ERROR) << L"Linker failure: " << strInfoLog;
            std::string infoLog = strInfoLog;
            delete[] strInfoLog;

            for (auto shader : shaders) {
                OGL_CALL(glDetachShader, program, shader);
            }
            OGL_CALL(glDeleteProgram, program);

            throw shader_compiler_error() << resid_info(id)
                << compiler_error_info(infoLog)
                << errdesc_info("Program linking failed.");
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
