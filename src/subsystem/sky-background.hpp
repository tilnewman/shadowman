#ifndef SUBSYSTEM_SKY_BACKGROUND_HPP_INCLUDED
#define SUBSYSTEM_SKY_BACKGROUND_HPP_INCLUDED
//
// sky-background.hpp
//
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{

    struct Context;

    struct CloudAnim
    {
        CloudAnim();

        sf::Sprite sprite;
        float speed;
    };

    struct SkyColorSet
    {
        sf::Color top{};
        sf::Color bottom{};
    };

    class SkyBackground
    {
      public:
        SkyBackground();

        void setup(const Context & t_context, const sf::Vector2f & t_size);
        void update(const Context & t_context, const float t_elapsedSec);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;
        void move(const sf::Vector2f & t_move);

      private:
        sf::FloatRect m_offscreenRect;
        std::vector<sf::Vertex> m_skyVerts;
        sf::Texture m_cloud1Texture;
        sf::Texture m_cloud2Texture;
        sf::Texture m_cloud3Texture;
        sf::Texture m_moonTexture;
        sf::Sprite m_moonSprite;
        sf::Texture m_sunTexture;
        sf::Sprite m_sunSprite;
        std::vector<CloudAnim> m_cloudAnims;
        bool m_willShowMoon;
        bool m_willShowSun;
        std::vector<SkyColorSet> m_skyColors;
    };

} // namespace shadowman

#endif // SUBSYSTEM_SKY_BACKGROUND_HPP_INCLUDED
