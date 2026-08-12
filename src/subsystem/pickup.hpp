#ifndef SUBSYSTEM_PICKUP_HPP_INCLUDED
#define SUBSYSTEM_PICKUP_HPP_INCLUDED
//
// pickup.hpp
//
#include <string>
#include <string_view>
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

    enum class Pickup : unsigned char
    {
        Heart = 0,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const Pickup t_pickup) noexcept
    {
        if (Pickup::Heart == t_pickup)
        {
            return "heart";
        }
        else
        {
            return "error_unknown_pickup";
        }
    }

    [[nodiscard]] inline Pickup fromString(const std::string & t_name) noexcept
    {
        if ("heart" == t_name)
        {
            return Pickup::Heart;
        }
        else
        {
            return Pickup::Count;
        }
    }

    class PickupAnim
    {
      public:
        PickupAnim(
            const Context & t_context,
            const Pickup t_type,
            const sf::Texture & t_texture,
            const sf::Vector2f & t_position);

        void update(const Context & t_context, const float t_elapsedSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        const sf::IntRect textureRect() const;
        std::size_t frameCount() const;

      private:
        Pickup m_type;
        sf::Sprite m_sprite;
        float m_animElpasedSec;
        std::size_t m_frameIndex;
    };

    class PickupManager
    {
      public:
        PickupManager();

        void setup(const Context & t_context);
        void update(const Context & t_context, const float t_elapsedSec);

        void add(
            const Context & t_context, const std::string & t_name, const sf::FloatRect & t_rect);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        sf::Texture m_heartTexture;
        std::vector<PickupAnim> m_anims;
    };

} // namespace shadowman

#endif // SUBSYSTEM_PICKUP_HPP_INCLUDED
