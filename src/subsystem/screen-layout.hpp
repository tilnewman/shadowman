#ifndef SUBSYSTEM_SCREEN_LAYOUT_HPP_INCLUDED
#define SUBSYSTEM_SCREEN_LAYOUT_HPP_INCLUDED
//
// screen-layout.hpp
//
#include <SFML/Graphics/Rect.hpp>

namespace shadowman
{

    struct Context;

    //

    enum class CalcOrder : unsigned char
    {
        Normal,
        Inverse
    };

    //

    class ScreenLayout
    {
      public:
        ScreenLayout();

        void setup(const sf::Vector2u & t_windowSize);

        [[nodiscard]] inline const sf::FloatRect wholeRect() const noexcept { return m_wholeRect; }
        [[nodiscard]] inline const sf::FloatRect mapRect() const noexcept { return m_mapRect; }

        [[nodiscard]] float scaleBasedOnResolution(
            const Context & t_context,
            const float t_originalScale,
            const CalcOrder t_order = CalcOrder::Normal) const;

      private:
        sf::FloatRect m_wholeRect;
        sf::FloatRect m_mapRect;
    };

} // namespace shadowman

#endif // SUBSYSTEM_SCREEN_LAYOUT_HPP_INCLUDED
