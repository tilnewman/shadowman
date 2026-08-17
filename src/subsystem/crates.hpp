#ifndef SUBSYSTEM_CRATES_HPP_INCLUDED
#define SUBSYSTEM_CRATES_HPP_INCLUDED
//
// crates.hpp
//
#include <vector>

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

    struct Crate
    {
        Crate(const sf::Texture & t_texture, const sf::FloatRect & t_rect);

        sf::Sprite sprite;
        sf::Vector2f velocity;
        bool is_landed;
    };

    class CrateManager
    {
      public:
        CrateManager();

        void setup(const Context & t_context);
        void update(const Context & t_context, const float t_elapsedSec);
        void add(const Context & t_context, const std::vector<sf::FloatRect> & t_rects);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        void appendCollisionRects(std::vector<sf::FloatRect> & t_rects) const;

        [[nodiscard]] std::optional<std::reference_wrapper<Crate>>
            findIntersecting(const sf::FloatRect & t_rect);

        constexpr void clear() noexcept { m_crates.clear(); }

      private:
        sf::Texture m_texture;
        std::vector<Crate> m_crates;
    };

} // namespace shadowman

#endif // SUBSYSTEM_CRATES_HPP_INCLUDED
