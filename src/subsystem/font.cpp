//
// font.cpp
//
#include "font.hpp"

#include "shadowman/settings.hpp"
#include "util/check-macros.hpp"
#include "util/sfml-util.hpp"

namespace shadowman
{

    FontManager::FontManager()
        : m_generalFont{}
        , m_generalExtents{}
        , m_anarchyFont{}
        , m_anarchyExtents{}
        , m_dashleyFont{}
        , m_dashleyExtents{}
    {}

    void FontManager::setup(const Settings & t_settings)
    {
        const auto loadFont = [&](sf::Font & font, const std::string & filename) {
            const std::string path{ (t_settings.media_path / "font" / filename).string() };
            const bool loadSuccess{ font.openFromFile(path) };
            M_CHECK(loadSuccess, "Failed to load font at: " << path);
        };

        loadFont(m_generalFont, "gentium-plus.ttf");
        loadFont(m_anarchyFont, "anarchysans.otf");
        loadFont(m_dashleyFont, "dashley.ttf");

        setupFontExtents(t_settings, Font::General, m_generalExtents);
        setupFontExtents(t_settings, Font::AnarchySans, m_anarchyExtents);
        setupFontExtents(t_settings, Font::Dashley, m_dashleyExtents);
    }

    const sf::Text FontManager::makeText(
        const Font t_font,
        const FontSize t_size,
        const std::string & t_string,
        const sf::Color & t_color,
        const sf::Text::Style t_style) const
    {
        sf::Text text(get(t_font), t_string, extent(t_font, t_size).char_size);
        text.setFillColor(t_color);
        text.setStyle(t_style);
        util::setOriginToPosition(text);

        return text;
    }

    const FontExtent FontManager::extent(const Font t_font, const FontSize t_size) const noexcept
    {
        const auto extentSize = [&](const FontExtentSet & set) {
            if (FontSize::Huge == t_size)
            {
                return set.huge;
            }
            else if (FontSize::Large == t_size)
            {
                return set.large;
            }
            else if (FontSize::Medium == t_size)
            {
                return set.medium;
            }
            else
            {
                return set.small;
            }
        };

        if (t_font == Font::AnarchySans)
        {
            return extentSize(m_anarchyExtents);
        }
        else if (t_font == Font::Dashley)
        {
            return extentSize(m_dashleyExtents);
        }
        else
        {
            return extentSize(m_generalExtents);
        }
    }

    void FontManager::setupFontExtents(
        const Settings & t_settings, const Font t_font, FontExtentSet & t_extentSet)
    {
        // All the magic numbers in this function are based on trial and error.
        // There was nothing magical about it...
        const float standardRes{ std::sqrt(3840.f * 2400.0f) };

        const float currentRes{ std::sqrt(
            static_cast<float>(t_settings.video_mode.size.x * t_settings.video_mode.size.y)) };

        const float ratioRes{ currentRes / standardRes };

        const std::string widthStr{ "M" };
        const std::string heightStr{ "|g" };

        t_extentSet.huge.char_size = static_cast<unsigned>(200.0f * ratioRes);

        t_extentSet.huge.letter_size.x =
            makeText(t_font, FontSize::Huge, widthStr).getGlobalBounds().size.x;

        t_extentSet.huge.letter_size.y =
            makeText(t_font, FontSize::Huge, heightStr).getGlobalBounds().size.y;

        t_extentSet.large.char_size = static_cast<unsigned>(90.0f * ratioRes);

        t_extentSet.large.letter_size.x =
            makeText(t_font, FontSize::Large, widthStr).getGlobalBounds().size.x;

        t_extentSet.large.letter_size.y =
            makeText(t_font, FontSize::Large, heightStr).getGlobalBounds().size.y;

        t_extentSet.medium.char_size = static_cast<unsigned>(60.0f * ratioRes);

        t_extentSet.medium.letter_size.x =
            makeText(t_font, FontSize::Medium, widthStr).getGlobalBounds().size.x;

        t_extentSet.medium.letter_size.y =
            makeText(t_font, FontSize::Medium, heightStr).getGlobalBounds().size.y;

        t_extentSet.small.char_size = static_cast<unsigned>(40.0f * ratioRes);

        t_extentSet.small.letter_size.x =
            makeText(t_font, FontSize::Small, widthStr).getGlobalBounds().size.x;

        t_extentSet.small.letter_size.y =
            makeText(t_font, FontSize::Small, heightStr).getGlobalBounds().size.y;
    }

} // namespace shadowman
