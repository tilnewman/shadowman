#ifndef ENEMY_FLY_HPP_INCLUDED
#define ENEMY_FLY_HPP_INCLUDED
//
// fly.hpp
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

    enum class FlyType : unsigned char
    {
        Beholder = 0,
        Chomp,
        Face,
        Horn,
        Peek,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const FlyType t_type) noexcept
    {
        // clang-format off
        switch(t_type)
        {
            case FlyType::Beholder: { return "beholder"; }
            case FlyType::Chomp:    { return "chomp";    }
            case FlyType::Face:     { return "face";     }
            case FlyType::Horn:     { return "horn";     }
            case FlyType::Peek:     { return "peek";     }
            case FlyType::Count:    
            default:   { return "error_unknown_flytype"; }
        }
        // clang-format on
    }

    enum class FlyTask : unsigned char
    {
        Idle,
        Wander,
        Chase,
        Death
    };

    [[nodiscard]] constexpr float timePerFrameSec(const FlyTask t_task) noexcept 
    {
        // clang-format off
        switch(t_task)
        {
            case FlyTask::Idle:     { return 0.08f; }
            case FlyTask::Wander:   { return 0.05f;  }
            case FlyTask::Chase:    { return 0.025f; }
            case FlyTask::Death:
            default:                { return 0.125f; }
        }
        // clang-format on
    }

    struct FlyTextures
    {
        std::vector<sf::Texture> fly{};
        std::vector<sf::Texture> die{};
    };

    class Fly
    {
      public:
        Fly(const Context & t_context,
            const FlyType t_type,
            const FlyTextures & t_textures,
            const sf::FloatRect & t_rect);

        [[nodiscard]] constexpr FlyType which() const noexcept { return m_type; }

        void update(const Context & t_context, const float t_elapsedSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

      private:
        void turn();
        void updateAnimation(const float t_elapsedSec);
        void startWandering(const Context & t_context);
        void startIdling(const Context & t_context);

      private:
        FlyType m_type;
        FlyTask m_task;
        bool m_isDying;
        sf::FloatRect m_rect;
        sf::Sprite m_sprite;
        bool m_isFacingRight;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        float m_idleElapsedSec;
        float m_idleDurationSec;
        float m_wanderTarget;
        const FlyTextures & m_textures;
    };

} // namespace shadowman

#endif // ENEMY_FLY_HPP_INCLUDED
