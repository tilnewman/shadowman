#ifndef SUBSYSTEM_SMOKE_HPP_INCLUDED
#define SUBSYSTEM_SMOKE_HPP_INCLUDED
//
// smoke.hpp
//
#include <string>
#include <vector>

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
    struct Settings;

    enum class SmokeType
    {
        Full,
        Still,
        Top
    };

    struct SmokeDetails
    {
        explicit SmokeDetails(const std::string & t_details);

        SmokeType type;
        std::uint8_t alpha;
    };

    struct SmokeAnim
    {
        SmokeAnim(
            const SmokeType t_type,
            const sf::Sprite & t_sprite,
            const std::size_t t_frameIndex,
            const sf::FloatRect & t_rect,
            const bool t_isFacingRight)
            : type{ t_type }
            , sprite{ t_sprite }
            , elapsed_time_sec{ 0.0f }
            , frame_index{ t_frameIndex }
            , rect{ t_rect }
            , is_facing_right{ t_isFacingRight }
        {}

        SmokeType type;
        sf::Sprite sprite;
        float elapsed_time_sec;
        std::size_t frame_index;
        sf::FloatRect rect;
        bool is_facing_right;
    };

    class SmokeManager
    {
      public:
        SmokeManager();

        void setup(const Context &);

        void
            add(const Context & t_context,
                const sf::FloatRect & t_region,
                const std::string & t_details);

        constexpr void clear() noexcept { m_animations.clear(); }
        void update(const Context & t_context, const float t_frameTimeSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        void move(const sf::Vector2f & t_move);

      private:
        std::vector<sf::Texture> m_textures;
        std::vector<SmokeAnim> m_animations;
    };

} // namespace shadowman

#endif // SUBSYSTEM_SMOKE_HPP_INCLUDED
