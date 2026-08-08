#ifndef SUBSYSTEM_CAVE_BACKGROUND_HPP_INCLUDED
#define SUBSYSTEM_CAVE_BACKGROUND_HPP_INCLUDED
//
// cave-background.hpp
//
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{

    struct Context;

    class CaveBackground
    {
      public:
        CaveBackground();

        void setup(const Context & t_context);
        void move(const float t_amount);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

      private:
        // returns true if it ended in a solid pillar
        bool composeBackground(
            const Context & t_context,
            sf::RenderTexture & t_offscreenTexture,
            sf::Sprite & t_sprite,
            const bool t_didPrevEndInPillar);

        // returns true if it ended in a solid pillar
        bool composePillars(
            const Context & t_context,
            sf::RenderTexture & t_offscreenTexture,
            int & t_horisPos,
            const bool t_didPrevEndInPillar);

        void composeStalactites(const Context & t_context, sf::RenderTexture & t_offscreenTexture);
        [[nodiscard]] const sf::IntRect getRandomPillarRectLeft(const Context & t_context) const;
        [[nodiscard]] const sf::IntRect getRandomPillarRectRight(const Context & t_context) const;

      private:
        sf::Texture m_caveBgTexture;
        sf::Texture m_stalactiteTexture;
        sf::Sprite m_sprite1;
        sf::Sprite m_sprite2;
        sf::RenderTexture m_offscreenTexture1;
        sf::RenderTexture m_offscreenTexture2;
        sf::Color m_backgroundColor;
        sf::Color m_pillarColor;
    };

} // namespace shadowman

#endif // SUBSYSTEM_CAVE_BACKGROUND_HPP_INCLUDED
