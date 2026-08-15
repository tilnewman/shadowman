#ifndef SUBYSTEM_FONT_HPP_INCLUDED
#define SUBYSTEM_FONT_HPP_INCLUDED
//
// font.hpp
//
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace shadowman
{

    struct Context;
    struct Settings;

    //

    enum class Font
    {
        General,
        AnarchySans,
        Dashley
    };

    //

    // restricting fonts to a subset of sizes saves vram and runtime
    enum class FontSize
    {
        Huge,
        Large,
        Medium,
        Small
    };

    //

    struct TextDetails
    {
        TextDetails()
            : font{ Font::General }
            , size{ FontSize::Medium }
            , color{ sf::Color::White }
            , style{ sf::Text::Regular }
            , will_center_horiz{ true }
            , will_center_vert{ true }
            , inner_pad_ratio{ 0.0f }
        {}

        TextDetails(
            const Font t_font,
            const FontSize t_size,
            const sf::Color & t_color     = sf::Color::White,
            const sf::Text::Style t_style = sf::Text::Regular,
            const bool t_willCenterHoriz  = true,
            const bool t_willCenterVert   = true,
            const float t_innerPadRatio   = 0.0f)
            : font{ t_font }
            , size{ t_size }
            , color{ t_color }
            , style{ t_style }
            , will_center_horiz{ t_willCenterHoriz }
            , will_center_vert{ t_willCenterVert }
            , inner_pad_ratio{ t_innerPadRatio }
        {}

        Font font;
        FontSize size;
        sf::Color color;
        sf::Text::Style style;
        bool will_center_horiz;
        bool will_center_vert;
        float inner_pad_ratio;
    };

    //

    struct FontExtent
    {
        unsigned int char_size{ 0 };
        sf::Vector2f letter_size{};
    };

    struct FontExtentSet
    {
        FontExtent small{};
        FontExtent medium{};
        FontExtent large{};
        FontExtent huge{};
    };

    //

    class FontManager
    {
      public:
        FontManager();

        void setup(const Settings & t_settings);

        [[nodiscard]] inline const sf::Font & get(const Font t_font) const noexcept
        {
            if (t_font == Font::Dashley)
            {
                return m_dashleyFont;
            }
            else if (t_font == Font::AnarchySans)
            {
                return m_anarchyFont;
            }
            else
            {
                return m_generalFont;
            }
        }

        [[nodiscard]] const sf::Text makeText(
            const Font t_font,
            const FontSize t_size,
            const std::string & t_text,
            const sf::Color & t_color     = sf::Color::White,
            const sf::Text::Style t_style = sf::Text::Regular) const;

        [[nodiscard]] inline const sf::Text
            makeText(const std::string & t_text, const TextDetails & t_details) const
        {
            return makeText(
                t_details.font, t_details.size, t_text, t_details.color, t_details.style);
        }

        [[nodiscard]] const FontExtent
            extent(const Font t_font, const FontSize t_size) const noexcept;

      private:
        void setupFontExtents(
            const Settings & t_settings, const Font t_font, FontExtentSet & t_extentSet);

      private:
        sf::Font m_generalFont;
        FontExtentSet m_generalExtents;

        sf::Font m_anarchyFont;
        FontExtentSet m_anarchyExtents;

        sf::Font m_dashleyFont;
        FontExtentSet m_dashleyExtents;
    };

} // namespace shadowman

#endif // SUBYSTEM_FONT_HPP_INCLUDED
