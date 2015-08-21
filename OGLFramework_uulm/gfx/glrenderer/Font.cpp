/**
 * @file   Font.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   6. Februar 2014
 *
 * @brief  Contains the implementation of Font.
 */

#include "Font.h"
#include "ShaderBufferBindingPoints.h"
#include "GPUProgram.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "../../core/boost_helper.h"
#include "app/ApplicationBase.h"
#include "app/Configuration.h"
#include "gfx/glrenderer/GLTexture.h"
#include "gfx/glrenderer/GLUniformBuffer.h"

namespace cgu {

    /**
     * Constructor.
     * @param fontName the name of the font to load
     * @param uniformBufferBindingPoints the ubo binding points
     */
    Font::Font(const std::string& fontName, ApplicationBase* app) :
        Resource(fontName, app),
        fontPages(),
        fontMetrics(),
        fontMetricsBindingPoint(app->GetUBOBindingPoints()->GetBindingPoint(fontMetricsUBBName))
    {
        fm.baseLine = 0.0f;
        fm.sizeNormalization = 1.0f;
    }

    Font::~Font()
    {
        if (IsLoaded()) UnloadLocal();
    }

    Font::Font(Font&& orig) :
        Resource(std::move(orig)),
        fontPages(std::move(orig.fontPages)),
        fm(std::move(orig.fm)),
        fontMetrics(std::move(orig.fontMetrics)),
        fontMetricsBindingPoint(orig.fontMetricsBindingPoint)
    {
    }

    Font& Font::operator =(Font&& orig)
    {
        Resource* tRes = this;
        *tRes = static_cast<Resource&&> (std::move(orig));
        if (this != &orig) {
            fontPages = std::move(orig.fontPages);
            fm = std::move(orig.fm);
            fontMetrics = std::move(orig.fontMetrics);
            fontMetricsBindingPoint = orig.fontMetricsBindingPoint;
        }
        return *this;
    }

    void Font::Load()
    {
        using boost::property_tree::ptree;
        ptree pt;

        boost::property_tree::read_xml(application->GetConfig().resourceBase + "/" + id + ".fnt", pt);
        unsigned int texWidth = 0, texHeight = 0;
        float fTexWidth = 0.0f, fTexHeight = 0.0f;
        float fontSize = 0.0f;

        FloatTranslator ft;
        for (const ptree::value_type& v : pt.get_child("font")) {
            if (v.first == "common") {
                fontSize = v.second.get<float>("<xmlattr>.lineHeight", ft);
                fm.sizeNormalization = 1.0f / fontSize;
                fm.baseLine = v.second.get<float>("<xmlattr>.base", ft) * fm.sizeNormalization;
                texWidth = v.second.get<unsigned int>("<xmlattr>.scaleW");
                fTexWidth = static_cast<float> (texWidth);
                texHeight = v.second.get<unsigned int>("<xmlattr>.scaleH");
                fTexHeight = static_cast<float> (texHeight);
                fm.pages.reserve(v.second.get<unsigned int>("<xmlattr>.pages"));
            } else if (v.first == "pages") {
                for (const ptree::value_type& pg : v.second) {
                    font_page page;
                    page.id = pg.second.get<unsigned int>("<xmlattr>.id");
                    page.filename = pg.second.get<std::string>("<xmlattr>.file");
                    fm.pages.push_back(page);
                }
            } else if (v.first == "chars") {
                unsigned int charCount = v.second.get<unsigned int>("<xmlattr>.count");
                assert(charCount == 96); // 95 printable + invalid ...
                fm.chars.reserve(charCount);
                for (const ptree::value_type& c : v.second) {
                    if (c.first != "char") continue;
                    font_glyph fg;
                    fg.id = static_cast<char> (c.second.get<unsigned int>("<xmlattr>.id"));
                    fg.metrics.pos = glm::vec2(c.second.get<float>("<xmlattr>.x", ft) / fTexWidth,
                        c.second.get<float>("<xmlattr>.y", ft) / fTexHeight);
                    fg.metrics.off = glm::vec2(c.second.get<float>("<xmlattr>.xoffset", ft),
                        c.second.get<float>("<xmlattr>.yoffset", ft));
                    float fWidth = c.second.get<float>("<xmlattr>.width", ft);
                    float fHeight = c.second.get<float>("<xmlattr>.height", ft);
                    float aspectRatio = fWidth / fHeight;
                    fg.metrics.heightInTex = fHeight / fTexHeight;
                    fg.metrics.heightInPixels = fHeight;
                    fg.metrics.aspectRatio = aspectRatio;
                    fg.metrics.page = static_cast<float> (c.second.get<unsigned int>("<xmlattr>.page"));
                    fg.xadv = c.second.get<float>("<xmlattr>.xadvance", ft) * fm.sizeNormalization;

                    fm.chars.push_back(fg);
                }
            }
        }

        TextureDescriptor texDesc(4, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
        fontPages.reset(new GLTexture(texWidth, texHeight, static_cast<unsigned int>(fm.pages.size()), texDesc));

        for (const font_page& page : fm.pages) {
            std::string filename = application->GetConfig().resourceBase + "/" + page.filename;
            fontPages->AddTextureToArray(filename, page.id);
        }

        fontMetrics.reset(new GLUniformBuffer(fontMetricsUBBName,
            sizeof(glyph_metrics) * static_cast<unsigned int>(fm.chars.size()),
            *application->GetUBOBindingPoints()));
        for (unsigned int i = 0; i < fm.chars.size(); ++i) {
            fontMetrics->UploadData(i * sizeof(glyph_metrics),
                sizeof(glyph_metrics), &fm.chars[i].metrics);
        }

        Resource::Load();
    }

    /**
     * Returns the id in the fm.chars array of a character.
     * @param character the character to get the id for
     * @return the characters id
     */
    unsigned int Font::GetCharacterId(char character) const
    {
        if (character < ' ' || character > '~') {
            return 0;
        } else {
            return (character - ' ') + 1;
        }
    }

    /**
     * Activates the font for use.
     */
    void Font::UseFont(GPUProgram* fontProgram, BindingLocation fontMetricsLocation) const
    {
        fontProgram->UseProgram();
        fontProgram->BindUniformBlock(fontMetricsLocation, fontMetricsBindingPoint);
        fontPages->ActivateTexture(GL_TEXTURE0);
    }

    /**
     * Returns the font metrics information.
     * @return the font metrics
     */
    const font_metrics& Font::GetFontMetrics() const
    {
        return fm;
    }

    void Font::UnloadLocal()
    {
        LOG(INFO) << L"Destructor call of font " << id.c_str();
        fontPages.reset();
        fontMetrics.reset();
        fm.chars.resize(0);
        fm.pages.resize(0);

    }

    void Font::Unload()
    {
        UnloadLocal();
        Resource::Unload();
    }
}
