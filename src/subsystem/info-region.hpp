#ifndef SUBSYSTEM_INFO_REGION_HPP_INCLUDED
#define SUBSYSTEM_INFO_REGION_HPP_INCLUDED
//
// info-region.hpp
//
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{

    struct Context;

    class InfoRegion
    {
      public:
        InfoRegion();

        void setup(const Context & t_context);
        void update(const Context & t_context);
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

      private:
        sf::FloatRect m_rect;
        sf::Texture m_heartBrightTexture;
        sf::Texture m_heartDarkTexture;
        std::vector<sf::Sprite> m_heartSprites;
    };

} // namespace shadowman

#endif // SUBSYSTEM_INFO_REGION_HPP_INCLUDED
