#ifndef SUBSYSTEM_FRAMERATE_DISPLAY_HPP_INCLUDE
#define SUBSYSTEM_FRAMERATE_DISPLAY_HPP_INCLUDE
//
// framerate-display.hpp
//
#include "subsystem/font.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{
    struct Context;

    class FramerateDisplay
    {
      public:
        FramerateDisplay();

        void update(const Context & t_context, const float t_elapsedSec);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

      private:
        sf::Text m_text;
        std::size_t m_frameCount;
        float m_elapsedSec;
    };

} // namespace shadowman

#endif // SUBSYSTEM_FRAMERATE_DISPLAY_HPP_INCLUDE
