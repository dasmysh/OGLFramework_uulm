/**
 * @file   MaterialLibrary.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   4. Januar 2014
 *
 * @brief  Contains the implementation of the MaterialLibrary.
 */

#include "MaterialLibrary.h"
#include "app/ApplicationBase.h"
#include "app/Configuration.h"
#include "gfx/glrenderer/GLTexture.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

#include <fstream>
#include <string>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <codecvt>

namespace cgu {

    /**
     * Constructor.
     * @param mtlFilename the material library file name
     * @param app the application object for resolving dependencies
     */
    MaterialLibrary::MaterialLibrary(const std::string& mtlFilename, ApplicationBase* app) :
        Resource(mtlFilename, app),
        materials()
    {
    }

    MaterialLibrary::~MaterialLibrary()
    {
        if (IsLoaded()) UnloadLocal();
    }

    void MaterialLibrary::Load()
    {
        MaterialType* currMat = nullptr;
        std::string currLine;
        std::ifstream inFile(application->GetConfig().resourceBase + "/" + this->id.c_str());

        if (!inFile.is_open()) {
            throw std::runtime_error("Could not open file: " + this->id);
        }

        boost::regex reg_newmtl("^newmtl\\s+(\\w+)$");
        boost::regex reg_Ka("^Ka\\s+" + regex_help::flt3 + "$");
        boost::regex reg_Kd("^Kd\\s+" + regex_help::flt3 + "$");
        boost::regex reg_Ks("^Ks\\s+" + regex_help::flt3 + "$");
        boost::regex reg_d("^d\\s+" + regex_help::flt + "$");
        boost::regex reg_d_halo("^d\\s+-halo\\s+" + regex_help::flt + "$");
        boost::regex reg_Ns("^Ns\\s+" + regex_help::flt + "$");
        boost::regex reg_Ni("^Ni\\s+" + regex_help::flt + "$");
        boost::regex reg_map_Kd("^map_Kd\\s+(.*\\s+)?([\\w-]+\\.\\w+)$");
        boost::regex reg_map_bump("^(map_bump|bump)\\s+(.*\\s+)?([\\w-]+\\.\\w+)$");

        boost::smatch lineMatch;

        while (inFile.good()) {
            std::getline(inFile, currLine);

            boost::trim(currLine);
            if (currLine.length() == 0 || boost::starts_with(currLine, "#"))
                continue; // comment or empty line
            else if (boost::regex_match(currLine, lineMatch, reg_newmtl)) {
                std::string mtlName = lineMatch[1].str();
                this->materials[mtlName] = std::unique_ptr<Material>(new Material());
                currMat = this->materials[mtlName].get();
            } else if (boost::regex_match(currLine, lineMatch, reg_Ka) && currMat) {
                currMat->ambient = parseColor(lineMatch);
            } else if (boost::regex_match(currLine, lineMatch, reg_Kd) && currMat) {
                currMat->diffuse = parseColor(lineMatch);
            } else if (boost::regex_match(currLine, lineMatch, reg_Ks) && currMat) {
                currMat->specular = parseColor(lineMatch);
            } else if (boost::regex_match(currLine, lineMatch, reg_d) && currMat) {
                currMat->alpha = boost::lexical_cast<float>(lineMatch[1].str());
            } else if (boost::regex_match(currLine, lineMatch, reg_d_halo) && currMat) {
                currMat->minOrientedAlpha = boost::lexical_cast<float>(lineMatch[1].str());
            } else if (boost::regex_match(currLine, lineMatch, reg_Ns) && currMat) {
                currMat->N_s = boost::lexical_cast<float>(lineMatch[1].str());
            } else if (boost::regex_match(currLine, lineMatch, reg_Ni) && currMat) {
                currMat->N_i = boost::lexical_cast<float>(lineMatch[1].str());
            } else if (boost::regex_match(currLine, lineMatch, reg_map_Kd) && currMat) {
                currMat->diffuseTex = parseTexture(lineMatch[2].str());
            } else if (boost::regex_match(currLine, lineMatch, reg_map_bump) && currMat) {
                currMat->bumpTex = parseTexture(lineMatch[3].str());
                currMat->bumpMultiplier = parseFloatParameter("-bm", lineMatch[2].str(), 1.0f);
            } else {
                notImplemented(currLine);
            }
        }
        inFile.close();

        Resource::Load();
    }

    void MaterialLibrary::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }

    /**
     * Unloads the local resources.
     */
    void MaterialLibrary::UnloadLocal()
    {

    }

    MaterialLibrary::MaterialType* MaterialLibrary::GetResource(const std::string & resId)
    {
        try {
            return materials.at(resId).get();
        }
        catch (std::out_of_range e) {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(INFO) << L"No resource with id \"" << converter.from_bytes(resId) << L"\" found. Creating new one.";
            std::unique_ptr<Material> matPtr(new Material());
            MaterialType* matRawPtr = matPtr.get();
            materials.insert(std::make_pair(resId, std::move(matPtr)));
            return matRawPtr;
        }
    }

    bool MaterialLibrary::HasResource(const std::string& resId)
    {
        MaterialMap::const_iterator got = materials.find(resId);
        if (got == materials.end())
            return false;
        return true;
    }

    /**
     * Logs a warning this feature is not implemented.
     * @param feature the line with the feature to log
     */
    void MaterialLibrary::notImplemented(const std::string & feature) const
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        LOG(WARNING) << L"Feature not implemented: " << converter.from_bytes(feature);
    }

    /**
     * Parses a color value.
     * @param matches the regex matcher for the color line
     * @return the color value as a vector
     */
    glm::vec3 MaterialLibrary::parseColor(const boost::smatch & matches) const
    {
        float r = boost::lexical_cast<float>(matches[1].str());
        float g = boost::lexical_cast<float>(matches[2].str());
        float b = boost::lexical_cast<float>(matches[3].str());
        return glm::vec3(r, g, b);
    }

    /**
     * Parses a texture.
     * @param matches the regex matcher for the texture lines
     * @return the loaded texture
     */
    const GLTexture2D* MaterialLibrary::parseTexture(const std::string& matches) const
    {
        boost::filesystem::path mtlFile{ id };
        std::string texFilename = mtlFile.parent_path().string() + "/" + matches;
        return application->GetTextureManager()->GetResource(texFilename);
    }

    /**
     *  Parses a float parameter.
     *  @param paramName the name of the parameter.
     *  @param matches the string containing the parameters.
     *  @param defaultValue the default value to return if the parameter was not found.
     */
    float MaterialLibrary::parseFloatParameter(const std::string& paramName, const std::string& matches, float defaultValue) const
    {
        boost::regex reg_bm("^.*-bm\\s+([-+]?[0-9]*\\.?[0-9]+).*$");
        boost::smatch lineMatch;
        if (boost::regex_match(matches, lineMatch, reg_bm)) {
            return boost::lexical_cast<float>(lineMatch[1]);
        }
        return defaultValue;

        /*if (matches[1].length() != 0) {
            notImplemented(matches[0].str());
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            LOG(WARNING) << L"Loading texture without parameters (" << converter.from_bytes(matches[1].str()) << ").";
        }*/
    }
}
