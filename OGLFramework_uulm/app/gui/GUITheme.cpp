/**
 * @file   GUITheme.cpp
 * @author Sebastian Maisch <sebastian.maisch@googlemail.com>
 * @date   24. Februar 2014
 *
 * @brief  Contains the implementation of GUITheme.
 */

#include "GUITheme.h"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "core/boost_helper.h"
#include "app/ApplicationBase.h"
#include "core/FontManager.h"

glm::vec2 parseTextRelative(const boost::property_tree::ptree::value_type& e);
guiinfo::box_info parseBoxInfo(const boost::property_tree::ptree& e,
        const std::string& elementName, float texWidth, float texHeight);
guiinfo::element_info parseElementInfo(const boost::property_tree::ptree& e,
        float texWidth, float texHeight);
std::array<std::unique_ptr<GUIRenderable>, 2> createRenderable(const guiinfo::element_info& element, GPUProgram* guiProgram);

GUITheme::GUITheme(const std::string& fontName, ApplicationBase* app) :
Resource(fontName, app)
{
}

GUITheme::~GUITheme()
{
    if (IsLoaded()) UnloadLocal();
}

void GUITheme::Load()
{
    font.reset(new guiinfo::font_info);
    button.reset(new guiinfo::button_info);
    inputText.reset(new guiinfo::input_text_info);
    slider.reset(new guiinfo::slider_info);
    checkbox.reset(new guiinfo::checkbox_info);
    radioButton.reset(new guiinfo::radio_button_info);

    GPUProgram* guiProgram = application->GetGUIProgram();
    font->fontProgram = application->GetFontProgram();
    button->fontInfo = font.get();
    button->guiProgram = guiProgram;
    inputText->fontInfo = font.get();
    inputText->guiProgram = guiProgram;
    slider->fontInfo = font.get();
    slider->guiProgram = guiProgram;
    checkbox->fontInfo = font.get();
    checkbox->guiProgram = guiProgram;
    radioButton->fontInfo = font.get();
    radioButton->guiProgram = guiProgram;
    using boost::property_tree::ptree;
    ptree pt;

    ColorTranslator ct;
    FloatTranslator ft;

    // TODO: need to implement more fields
    float tw = 1.0f, th = 1.0f;
    boost::property_tree::read_xml(application->GetConfig().resourceBase + "/" + id + ".xml", pt);
    for (const ptree::value_type& v : pt.get_child("gui-template")) {
        if (v.first == "font") {
            font->color = v.second.get<glm::vec3>("<xmlattr>.fontColor", ct);
            font->fontSize = v.second.get<float>("<xmlattr>.fontSize", ft);
            font->fontWeight = v.second.get<float>("<xmlattr>.fontWeight", ft);
            font->fontShearing = v.second.get<float>("<xmlattr>.fontShearing", ft);
            font->font = application->GetFontManager()->GetResource(v.second.data());
            cursor.reset(new SimpleQuadRenderable(std::array<glm::vec2, 4>{glm::vec2(0.0f, 0.0f),
                glm::vec2(0.0f, font->fontSize), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, font->fontSize)},
            simpleProg));
        } else if (v.first == "elements") {
            themeTexture = application->GetTextureManager()->GetResource(
                    v.second.get<std::string>("<xmlattr>.theme"));
            tw = v.second.get<float>("<xmlattr>.width", ft);
            th = v.second.get<float>("<xmlattr>.height", ft);
            for (const ptree::value_type& e : v.second) {
                if (e.first == "button") {
                    button->textRel = parseTextRelative(e);
                    button->height = e.second.get<float>("<xmlattr>.height", ft);
                    button->left = parseElementInfo(e.second.get_child("leftBorder"), tw, th);
                    button->right = parseElementInfo(e.second.get_child("rightBorder"), tw, th);
                } else if (e.first == "inputText") {
                    inputText->textRel = parseTextRelative(e);
                    inputText->height = e.second.get<float>("<xmlattr>.height", ft);
                    inputText->left = parseElementInfo(e.second.get_child("leftBorder"), tw, th);
                    inputText->right = parseElementInfo(e.second.get_child("rightBorder"), tw, th);
                } else if (e.first == "slider") {
                    slider->textRel = parseTextRelative(e);
                    slider->height = e.second.get<float>("<xmlattr>.height", ft);
                    slider->left = parseElementInfo(e.second.get_child("leftBorder"), tw, th);
                    slider->right = parseElementInfo(e.second.get_child("rightBorder"), tw, th);
                    sliderMarker = createRenderable(parseElementInfo(e.second.get_child("marker"), tw, th), guiProgram);
                    slider->marker[0] = sliderMarker[0].get();
                    slider->marker[1] = sliderMarker[1].get();
                } else if (e.first == "checkbox") {
                    checkbox->textRel = parseTextRelative(e);
                    checkbox->height = e.second.get<float>("<xmlattr>.height", ft);
                    checkboxChecker = createRenderable(parseElementInfo(e.second.get_child("checker"), tw, th), guiProgram);
                    checkboxChecked = createRenderable(parseElementInfo(e.second.get_child("checkerChecked"), tw, th), guiProgram);
                    checkbox->checker[0] = checkboxChecker[0].get();
                    checkbox->checkerChecked[0] = checkboxChecked[0].get();
                    checkbox->checker[1] = checkboxChecker[1].get();
                    checkbox->checkerChecked[1] = checkboxChecked[1].get();
                } else if (e.first == "radioButton") {
                    radioButton->textRel = parseTextRelative(e);
                    radioButton->height = e.second.get<float>("<xmlattr>.height", ft);
                    radioButtonChecker = createRenderable(parseElementInfo(e.second.get_child("checker"), tw, th), guiProgram);
                    radioButtonChecked = createRenderable(parseElementInfo(e.second.get_child("checkerChecked"), tw, th), guiProgram);
                    radioButton->checker[0] = radioButtonChecker[0].get();
                    radioButton->checkerChecked[0] = radioButtonChecked[0].get();
                    radioButton->checker[1] = radioButtonChecker[1].get();
                    radioButton->checkerChecked[1] = radioButtonChecked[1].get();
                }
            }
        }
    }


    button->themeTexture = themeTexture;
    inputText->themeTexture = themeTexture;
    slider->themeTexture = themeTexture;
    checkbox->themeTexture = themeTexture;
    radioButton->themeTexture = themeTexture;
    Resource::Load();
}

void GUITheme::Unload()
{
    UnloadLocal();
    Resource::Unload();
}

void GUITheme::UnloadLocal()
{
    font.reset();
    button.reset();
    inputText.reset();
    slider.reset();
    checkbox.reset();
    radioButton.reset();
    themeTexture = nullptr;
    sliderMarker[0].reset();
    sliderMarker[1].reset();
    checkboxChecker[0].reset();
    checkboxChecker[1].reset();
    checkboxChecked[0].reset();
    checkboxChecked[1].reset();
    radioButtonChecker[0].reset();
    radioButtonChecker[1].reset();
    radioButtonChecked[0].reset();
    radioButtonChecked[1].reset();
    cursor.reset();
}

const guiinfo::font_info& GUITheme::GetFontInfo() const
{
    return *font.get();
}

const guiinfo::button_info& GUITheme::GetButtonInfo() const
{
    return *button.get();
}

const guiinfo::input_text_info& GUITheme::GetInputTextInfo() const
{
    return *inputText.get();
}

const guiinfo::slider_info& GUITheme::GetSliderInfo() const
{
    return *slider.get();
}

const guiinfo::checkbox_info& GUITheme::GetCheckboxInfo() const
{
    return *checkbox.get();
}

const guiinfo::radio_button_info& GUITheme::GetRadioButtonInfo() const
{
    return *radioButton.get();
}

glm::vec2 parseTextRelative(const boost::property_tree::ptree::value_type& e)
{
    FloatTranslator ft;

    return glm::vec2(e.second.get<float>("<xmlattr>.textRelX", ft),
            e.second.get<float>("<xmlattr>.textRelY", ft));
}

guiinfo::box_info parseBoxInfo(const boost::property_tree::ptree& e,
        const std::string& elementName, float texWidth, float texHeight)
{
    FloatTranslator ft;

    guiinfo::box_info box;
    box.position = glm::vec2(e.get<float>(elementName + ".<xmlattr>.x", ft) / texWidth,
            e.get<float>(elementName + ".<xmlattr>.y", ft) / texHeight);
    box.size = glm::vec2(e.get<float>(elementName + ".<xmlattr>.width", ft) / texWidth,
            e.get<float>(elementName + ".<xmlattr>.height", ft) / texHeight);
    return box;
}

guiinfo::element_info parseElementInfo(const boost::property_tree::ptree& e,
        float texWidth, float texHeight)
{
    FloatTranslator ft;

    guiinfo::element_info element;
    element.size.x = e.get("<xmlattr>.width", 0.0f, ft);
    element.size.y = e.get<float>("<xmlattr>.height", ft);
    element.box[0] = parseBoxInfo(e, "box", texWidth, texHeight);
    element.box[1] = parseBoxInfo(e, "box-focused", texWidth, texHeight);
    return element;
}

std::array<std::unique_ptr<GUIRenderable>, 2> createRenderable(const guiinfo::element_info& element, GPUProgram* guiProgram)
{
    std::vector<GUIVertex> vertices[2];
    for (unsigned int i = 0; i < 2; ++i) {
        vertices[i].resize(8);
        vertices[i][0].pos = glm::vec3(0.0f, 0.0f, 0.0f);
        vertices[i][0].texCoords = glm::vec2(element.box[i].position.x, element.box[i].position.y);
        vertices[i][1].pos = glm::vec3(0.0f, element.size.y, 0.0f);
        vertices[i][1].texCoords = glm::vec2(element.box[i].position.x,
                element.box[i].position.y + element.box[i].size.y);
        vertices[i][2].pos = glm::vec3(element.size.x, 0.0f, 0.0f);
        vertices[i][2].texCoords = glm::vec2(element.box[i].position.x,
                element.box[i].position.y + element.box[i].size.y);
        vertices[i][3].pos = glm::vec3(element.size.x, element.size.y, 0.0f);
        vertices[i][3].texCoords = glm::vec2(element.box[i].position.x + element.box[i].size.x,
                element.box[i].position.y + element.box[i].size.y);
    }
    std::vector<unsigned int> indices{0, 1, 2, 2, 1, 3};
    return std::array<std::unique_ptr<GUIRenderable>, 2>{
        {std::unique_ptr<GUIRenderable>(new GUIRenderable(vertices[0], indices, guiProgram)),
            std::unique_ptr<GUIRenderable>(new GUIRenderable(vertices[1], indices, guiProgram))}
    };
}
