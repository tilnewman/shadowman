#ifndef ENEMY_FLY_MANAGER_HPP_INCLUDED
#define ENEMY_FLY_MANAGER_HPP_INCLUDED
//
// fly-manager.hpp
//
#include "enemy/fly.hpp"

#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{

    struct Context;

    class FlyManager
    {
      public:
        FlyManager();

        void setup(const Context & t_context);
        void update(const Context & t_context, const float t_elapsedSec);
        void add(const Context & t_context, const sf::FloatRect & t_rect);
        void appendCollisionRects(std::vector<sf::FloatRect> & t_rects) const;
        constexpr void clear() noexcept { m_flies.clear(); }

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        std::vector<Fly> m_flies;
        std::vector<FlyTextures> m_textures;
    };

} // namespace shadowman

#endif // ENEMY_FLY_MANAGER_HPP_INCLUDED
