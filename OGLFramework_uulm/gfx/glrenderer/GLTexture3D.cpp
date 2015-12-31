/**
 * @file   GLTexture3D.cpp
 * @author Sebastian Maisch <sebastian.maisch@uni-ulm.de>
 * @date   2015.08.14
 *
 * @brief  Contains the implementation of GLTexture3D.
 */

#include "GLTexture3D.h"
#include "app/ApplicationBase.h"
#include "app/Configuration.h"
#include <codecvt>
#include <fstream>
#include "GLTexture.h"
#include <boost/assign.hpp>
#include "gfx/volumes/VolumeBrickOctree.h"
#include <ios>
#include <boost/filesystem.hpp>

#undef min
#undef max

namespace cgu {

    /**
     * Constructor.
     * @param texFilename the textures file name
     * @param app the application object
     */
    GLTexture3D::GLTexture3D(const std::string& texFilename, ApplicationBase* app) :
        Resource{ texFilename, app },
        texture(),
        volumeSize(0),
        cellSize(1.0f),
        scaleValue(1),
        dataDim(1),
        texDesc(4, GL_R8, GL_RED, GL_UNSIGNED_BYTE),
        fileStream(nullptr)
    {
    }

    /** Copy constructor. */
    GLTexture3D::GLTexture3D(const GLTexture3D& rhs) : GLTexture3D(rhs.id, rhs.application)
    {
        if (rhs.IsLoaded()) GLTexture3D::Load();
    }

    /** Copy assignment operator. */
    GLTexture3D& GLTexture3D::operator=(const GLTexture3D& rhs)
    {
        auto tmp(rhs);
        std::swap(texture, tmp.texture);
        std::swap(volumeSize, tmp.volumeSize);
        std::swap(cellSize, tmp.cellSize);
        std::swap(rawFileName, tmp.rawFileName);
        std::swap(scaleValue, tmp.scaleValue);
        std::swap(dataDim, tmp.dataDim);
        std::swap(texDesc, tmp.texDesc);
        std::swap(fileStream, tmp.fileStream);
        std::swap(data, tmp.data);
        return *this;
    }

    /** Move constructor. */
    GLTexture3D::GLTexture3D(GLTexture3D&& rhs) :
        Resource(std::move(rhs)),
        texture(std::move(rhs.texture)),
        volumeSize(std::move(rhs.volumeSize)),
        cellSize(std::move(rhs.cellSize)),
        rawFileName(std::move(rhs.rawFileName)),
        scaleValue(std::move(rhs.scaleValue)),
        dataDim(std::move(rhs.dataDim)),
        texDesc(std::move(rhs.texDesc)),
        fileStream(std::move(rhs.fileStream)),
        data(std::move(rhs.data))
    {
        
    }
    GLTexture3D& GLTexture3D::operator=(GLTexture3D&& rhs)
    {
        this->~GLTexture3D();
        texture = std::move(rhs.texture);
        volumeSize = std::move(rhs.volumeSize);
        cellSize = std::move(rhs.cellSize);
        rawFileName = std::move(rhs.rawFileName);
        scaleValue = std::move(rhs.scaleValue);
        dataDim = std::move(rhs.dataDim);
        texDesc = std::move(rhs.texDesc);
        fileStream = std::move(rhs.fileStream);
        data = std::move(rhs.data);
        return *this;
    }

    /** Destructor. */
    GLTexture3D::~GLTexture3D()
    {
        if (IsLoaded()) UnloadLocal();
    }

    void GLTexture3D::Load()
    {
        auto filename = application->GetConfig().resourceBase + "/" + GetParameters()[0];
        boost::filesystem::path datFile{ filename };
        auto path = datFile.parent_path().string() + "/";
        auto ending = datFile.extension().string();

        if (ending != ".dat" && ending != ".DAT") {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "Cannot load '" << converter.from_bytes(ending) << "' Only .dat files are supported.";
            throw resource_loading_error() << ::boost::errinfo_file_name(datFile.filename().string()) << resid_info(id)
                << errdesc_info("Cannot load file, file type not supported.");
        }

        std::ifstream ifs(filename);
        if (!ifs.is_open()) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "Cannot open file '" << converter.from_bytes(filename) << "'.";
            throw resource_loading_error() << ::boost::errinfo_file_name(datFile.filename().string()) << resid_info(id)
                << errdesc_info("Cannot open file.");
        }

        std::string str, raw_file, format_str, obj_model;

        while (ifs >> str && ifs.good()) {
            if (str == "ObjectFileName:")
                ifs >> raw_file;
            else if (str == "Resolution:") {
                ifs >> volumeSize.x; ifs >> volumeSize.y; ifs >> volumeSize.z;
            } else if (str == "SliceThickness:") {
                ifs >> cellSize.x; ifs >> cellSize.y; ifs >> cellSize.z;
            } else if (str == "Format:")
                ifs >> format_str;
            else if (str == "ObjectModel:")
                ifs >> obj_model;
        }
        ifs.close();

        if (raw_file == "" || volumeSize == glm::uvec3(0) || format_str == "") {
            LOG(ERROR) << "Could find all required fields in dat file.";
            throw resource_loading_error() << ::boost::errinfo_file_name(datFile.filename().string()) << resid_info(id)
                << errdesc_info("Cannot find all required fields in dat file.");
        }

        unsigned int componentSize;
        if (format_str == "UCHAR") {
            texDesc.type = GL_UNSIGNED_BYTE;
            componentSize = 1;
        } else if (format_str == "USHORT") {
            texDesc.type = GL_UNSIGNED_SHORT;
            componentSize = 2;
        } else if (format_str == "USHORT_12") {
            texDesc.type = GL_UNSIGNED_SHORT;
            componentSize = 2;
        } else if (format_str == "UINT") {
            texDesc.type = GL_UNSIGNED_INT;
            componentSize = 4;
        } else {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "Format '" << converter.from_bytes(format_str) << "' is not supported.";
            throw resource_loading_error() << ::boost::errinfo_file_name(datFile.filename().string()) << resid_info(id)
                << errdesc_info("Format not supported.");
        }

        if (obj_model == "I") {
            dataDim = 1;
            texDesc.format = GL_RED;
        } else if (obj_model == "RG" || obj_model == "XY") {
            dataDim = 2;
            texDesc.format = GL_RG;
        } else if (obj_model == "RGB" || obj_model == "XYZ") {
            dataDim = 3;
            texDesc.format = GL_RGB;
        } else if (obj_model == "RGBA" || obj_model == "XYZW") {
            dataDim = 4;
            texDesc.format = GL_RGBA;
        } else {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "ObjectModel '" << converter.from_bytes(obj_model) << "' is not supported.";
            throw resource_loading_error() << ::boost::errinfo_file_name(datFile.filename().string()) << resid_info(id)
                << errdesc_info("ObjectModel not supported.");
        }

        texDesc.bytesPP = dataDim * componentSize;
        if (texDesc.type == GL_UNSIGNED_BYTE && texDesc.format == GL_RED)
            texDesc.internalFormat = GL_R8;
        else if (texDesc.type == GL_UNSIGNED_BYTE && texDesc.format == GL_RG)
            texDesc.internalFormat = GL_RG8;
        else if (texDesc.type == GL_UNSIGNED_BYTE && texDesc.format == GL_RGB)
            texDesc.internalFormat = GL_RGB8;
        else if (texDesc.type == GL_UNSIGNED_BYTE && texDesc.format == GL_RGBA)
            texDesc.internalFormat = GL_RGBA8;
        else if (texDesc.type == GL_UNSIGNED_SHORT && texDesc.format == GL_RED)
            texDesc.internalFormat = GL_R16F;
        else if (texDesc.type == GL_UNSIGNED_SHORT && texDesc.format == GL_RG)
            texDesc.internalFormat = GL_RG16F;
        else if (texDesc.type == GL_UNSIGNED_SHORT && texDesc.format == GL_RGB)
            texDesc.internalFormat = GL_RGB16F;
        else if (texDesc.type == GL_UNSIGNED_SHORT && texDesc.format == GL_RGBA)
            texDesc.internalFormat = GL_RGBA16F;
        else if (texDesc.type == GL_UNSIGNED_INT && texDesc.format == GL_RED)
            texDesc.internalFormat = GL_R32F;
        else if (texDesc.type == GL_UNSIGNED_INT && texDesc.format == GL_RG)
            texDesc.internalFormat = GL_RG32F;
        else if (texDesc.type == GL_UNSIGNED_INT && texDesc.format == GL_RGB)
            texDesc.internalFormat = GL_RGB32F;
        else if (texDesc.type == GL_UNSIGNED_INT && texDesc.format == GL_RGBA)
            texDesc.internalFormat = GL_RGBA32F;

        scaleValue = (format_str == "USHORT_12") ? 16 : 1;
        rawFileName = path + "/" + raw_file;

        Resource::Load();
    }

    /**
    * Unloads the local resources.
    */
    void GLTexture3D::UnloadLocal()
    {
        texture.reset();
        data.clear();
    }

    /**
     *  Loads the content of the volume to a single texture.
     *  @return the loaded texture.
     */
    GLTexture* GLTexture3D::LoadToSingleTexture()
    {
        std::ifstream ifsRaw(rawFileName, std::ios::in | std::ios::binary);
        if (!ifsRaw || !ifsRaw.is_open()) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "Could not open file '" << converter.from_bytes(rawFileName) << "'.";
            throw std::runtime_error("Could not open file '" + rawFileName + "'.");
        }

        ifsRaw.seekg(0, std::ios::end);
        auto data_size = static_cast<unsigned int>(ifsRaw.tellg());
        ifsRaw.seekg(0, std::ios::beg);

        std::vector<char> rawData(data_size, 9);
        ifsRaw.read(rawData.data(), data_size);
        ifsRaw.close();

        auto volumeNumBytes = volumeSize.x * volumeSize.y * volumeSize.z * texDesc.bytesPP;
        data.resize(volumeNumBytes);

        if (texDesc.type == GL_UNSIGNED_BYTE) {
            int size = std::min(data_size, volumeNumBytes);
            auto ptr = reinterpret_cast<uint8_t*>(rawData.data());
            for (auto i = 0; i < size; ++i) {
                data[i] = ptr[i];
            }
        } else if (texDesc.type == GL_UNSIGNED_SHORT) {
            auto elementSize = static_cast<unsigned int>(sizeof(uint16_t));
            auto size = std::min(data_size, volumeNumBytes) / elementSize;
            auto ptr = reinterpret_cast<uint16_t*>(rawData.data());
            for (unsigned int i = 0; i < size; ++i) {
                reinterpret_cast<uint16_t*>(data.data())[i] = ptr[i] * scaleValue;
            }
        } else if (texDesc.type == GL_UNSIGNED_INT) {
            auto elementSize = static_cast<unsigned int>(sizeof(uint32_t));
            auto size = std::min(data_size, volumeNumBytes) / elementSize;
            auto ptr = reinterpret_cast<uint32_t*>(rawData.data());
            for (unsigned int i = 0; i < size; ++i) {
                reinterpret_cast<uint32_t*>(data.data())[i] = ptr[i];
            }
        }

        texture = std::make_unique<GLTexture>(volumeSize.x, volumeSize.y, volumeSize.z, texDesc, data.data());

        return texture.get();
    }

    /** Returns the texture object. */
    GLTexture* GLTexture3D::GetTexture() const
    {
        return texture.get();
    }

    void GLTexture3D::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }

    std::unique_ptr<VolumeBrickOctree> GLTexture3D::GetBrickedVolume(const glm::vec3& scale)
    {
        assert(texDesc.format == GL_RED || texDesc.format == GL_RED_INTEGER);
        std::ifstream ifs(rawFileName, std::ios::binary);
        if (!ifs.is_open()) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(ERROR) << "Could not open file '" << converter.from_bytes(rawFileName) << "'.";
            throw std::runtime_error("Could not open file '" + rawFileName + "'.");
        }
        fileStream = &ifs;

        GPUProgram* minMaxProg;
        if (texDesc.bytesPP == 1) minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxMipMaps8.cp");
        else if (texDesc.bytesPP == 2) minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxMipMaps16.cp");
        else if (texDesc.bytesPP == 4) minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxMipMaps32.cp");
        else throw std::runtime_error("Texture bit depth is not supported for min/max octrees.");
        auto uniformNames = minMaxProg->GetUniformLocations(boost::assign::list_of<std::string>("origTex")("nextLevelTex"));
        std::unique_ptr<VolumeBrickOctree> result{ new VolumeBrickOctree(this, glm::uvec3(0), volumeSize,
            scale * cellSize, minMaxProg, uniformNames, application) };

        fileStream = nullptr;
        return std::move(result);
    }

    void GLTexture3D::FillRaw(std::vector<uint8_t>& data, const glm::uvec3& pos, const glm::uvec3& dataSize, const glm::uvec3& texSize) const
    {
        assert(fileStream != nullptr);
        assert(texDesc.format == GL_RED || texDesc.format == GL_RED_INTEGER);

        data.resize(texSize.x * texSize.y * texSize.z * texDesc.bytesPP, 0);
        auto lineSize = volumeSize.x * texDesc.bytesPP;
        auto dataPtr = reinterpret_cast<char*>(data.data());

        for (unsigned int z = 0; z < dataSize.z; ++z) {
            for (unsigned int y = 0; y < dataSize.y; ++y) {
                auto lineStartFile = ((pos.z + z) * volumeSize.y * lineSize) + ((pos.y + y) * lineSize) + pos.x * texDesc.bytesPP;
                fileStream->seekg(lineStartFile, std::ios::beg);
                fileStream->read(dataPtr, dataSize.x * texDesc.bytesPP);
                dataPtr += texSize.x * texDesc.bytesPP;
            }
        }

        if (scaleValue != 1) {
            for (auto& val : data) {
                val *= scaleValue;
            }
        }
    }

    std::unique_ptr<GLTexture> GLTexture3D::CreateMinMaxTexture(const glm::uvec3& pos, const glm::uvec3& size,
        TextureDescriptor& minMaxDesc) const
    {
        assert(texDesc.format == GL_RED || texDesc.format == GL_RED_INTEGER);
        std::vector<uint8_t> dataRaw;
        auto dataSize = glm::min(size, volumeSize - pos);
        glm::uvec3 actualSize;
        if (dataSize.x == 1) actualSize.x = 2;
        else actualSize.x = cguMath::roundupPow2(dataSize.x);
        if (dataSize.y == 1) actualSize.y = 2;
        else actualSize.y = cguMath::roundupPow2(dataSize.y);
        if (dataSize.z == 1) actualSize.z = 2;
        else actualSize.z = cguMath::roundupPow2(dataSize.z);

        FillRaw(dataRaw, pos, dataSize, actualSize);

        GPUProgram* minMaxProg;
        minMaxDesc.bytesPP = texDesc.bytesPP * 4;
        minMaxDesc.format = GL_RGBA;
        minMaxDesc.type = texDesc.type;
        if (texDesc.type == GL_UNSIGNED_BYTE) {
            minMaxDesc.internalFormat = GL_RGBA8;
            minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxTexture8.cp");
        } else if (texDesc.type == GL_UNSIGNED_SHORT) {
            minMaxDesc.internalFormat = GL_RGBA16F;
            minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxTexture16.cp");
        } else if (texDesc.type == GL_UNSIGNED_INT) {
            minMaxDesc.internalFormat = GL_RGBA32F;
            minMaxProg = application->GetGPUProgramManager()->GetResource("genMinMaxTexture32.cp");
        } else throw std::runtime_error("Pixel-type not supported.");
        auto uniformNames = minMaxProg->GetUniformLocations(boost::assign::list_of<std::string>("origTex")("minMaxTex"));

        GLTexture origTex(actualSize.x, actualSize.y, actualSize.z, texDesc, dataRaw.data());
        std::unique_ptr<GLTexture> result{ new GLTexture(actualSize.x, actualSize.y, actualSize.z, minMaxDesc, nullptr) };

        minMaxProg->UseProgram();
        minMaxProg->SetUniform(uniformNames[0], 0);
        minMaxProg->SetUniform(uniformNames[1], 1);

        auto numGroups = glm::ivec3(glm::ceil(glm::vec3(actualSize) / 8.0f));
        origTex.ActivateImage(0, 0, GL_READ_ONLY);
        result->ActivateImage(1, 0, GL_WRITE_ONLY);
        OGL_CALL(glDispatchCompute, numGroups.x, numGroups.y, numGroups.z);
        OGL_CALL(glMemoryBarrier, GL_ALL_BARRIER_BITS);
        OGL_SCALL(glFinish);

        return std::move(result);
    }
}
