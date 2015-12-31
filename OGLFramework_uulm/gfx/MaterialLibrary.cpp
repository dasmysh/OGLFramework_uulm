/**
 * @file   MaterialLibrary.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   2014.01.
 *
 * @brief  Contains the implementation of the MaterialLibrary.
 */

#include "MaterialLibrary.h"
#include "app/ApplicationBase.h"
#include "app/Configuration.h"

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
     * @param mtlFilename the material library file name.
     * @param app the application object for resolving dependencies.
     */
    MaterialLibrary::MaterialLibrary(const std::string& mtlFilename, ApplicationBase* app) :
        Resource(mtlFilename, app),
        ResourceManagerBase(app)
    {
    }

    /** Default copy constructor. */
    MaterialLibrary::MaterialLibrary(const MaterialLibrary&) = default;
    /** Default copy assignment operator. */
    MaterialLibrary& MaterialLibrary::operator=(const MaterialLibrary&) = default;

    /** Default move constructor. */
    MaterialLibrary::MaterialLibrary(MaterialLibrary&& rhs) : Resource(std::move(rhs)), ResourceManagerBase(std::move(rhs)) {}

    /** Default move assignment operator. */
    MaterialLibrary& MaterialLibrary::operator=(MaterialLibrary&& rhs)
    {
        this->~MaterialLibrary();
        Resource* tRes = this;
        *tRes = static_cast<Resource&&>(std::move(rhs));
        ResourceManagerBase* tResMan = this;
        *tResMan = static_cast<ResourceManagerBase&&>(std::move(rhs));
        return *this;
    }

    /** Default destructor. */
    MaterialLibrary::~MaterialLibrary() = default;

    void MaterialLibrary::Load()
    {
        ResourceType* currMat = nullptr;
        std::string currLine;
        auto filename = Resource::application->GetConfig().resourceBase + "/" + GetParameters()[0];
        std::ifstream inFile(filename);

        if (!inFile.is_open()) {
            throw resource_loading_error() << ::boost::errinfo_file_name(filename) << resid_info(id) << errdesc_info("Cannot open file.");
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
            if (boost::regex_match(currLine, lineMatch, reg_newmtl)) {
                auto mtlName = lineMatch[1].str();
                currMat = SetResource(mtlName, std::move(std::make_unique<Material>()));
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
                currMat->diffuseTex = parseTexture(lineMatch[2].str(), "sRGB");
            } else if (boost::regex_match(currLine, lineMatch, reg_map_bump) && currMat) {
                currMat->bumpTex = parseTexture(lineMatch[3].str(), "");
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
        Resource::Unload();
    }

    /**
     * Logs a warning this feature is not implemented.
     * @param feature the line with the feature to log
     */
    void MaterialLibrary::notImplemented(const std::string & feature)
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
        auto r = boost::lexical_cast<float>(matches[1].str());
        auto g = boost::lexical_cast<float>(matches[2].str());
        auto b = boost::lexical_cast<float>(matches[3].str());
        return glm::vec3(r, g, b);
    }

    /**
     * Parses a texture.
     * @param matches the regex matcher for the texture lines
     * @return the loaded texture
     */
    const GLTexture2D* MaterialLibrary::parseTexture(const std::string& matches, const std::string& params) const
    {
        boost::filesystem::path mtlFile{ id };
        auto texFilename = mtlFile.parent_path().string() + "/" + matches + (params.size() > 0 ? "," + params : "");
        return Resource::application->GetTextureManager()->GetResource(texFilename);
    }

    /**
     *  Parses a float parameter.
     *  @param paramName the name of the parameter.
     *  @param matches the string containing the parameters.
     *  @param defaultValue the default value to return if the parameter was not found.
     */
    float MaterialLibrary::parseFloatParameter(const std::string& paramName, const std::string& matches, float defaultValue) const
    {
        auto regexString = "^.*" + paramName + "\\s+([-+]?[0-9]*\\.?[0-9]+).*$";
        boost::regex reg_bm(regexString);
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
