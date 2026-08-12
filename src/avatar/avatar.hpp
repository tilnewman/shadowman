#ifndef AVATAR_AVATAR_HPP_INCLUDED
#define AVATAR_AVATAR_HPP_INCLUDED
//
// avatar.hpp
//
#include <string_view>
#include <vector>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{
    struct Context;

    enum class AvatarAnim : unsigned char
    {
        Die = 0,
        Idle,
        IdleLook,
        Jump,
        Hurt,
        Pull,
        Push,
        Run,
        Slash,
        Slash2,
        Stab,
        Stab2,
        Walk,
        WalkSlash,
        WalkSneak,
        WalkStab,
        WalkStab2,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const AvatarAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case AvatarAnim::Die:       { return "die";        }
            case AvatarAnim::Idle:      { return "idle";       }
            case AvatarAnim::IdleLook:  { return "idle-look";  }
            case AvatarAnim::Jump:      { return "jump";       }
            case AvatarAnim::Hurt:      { return "hurt";       }
            case AvatarAnim::Pull:      { return "pull";       }
            case AvatarAnim::Push:      { return "push";       }
            case AvatarAnim::Run:       { return "run";        }
            case AvatarAnim::Slash:     { return "slash";      }
            case AvatarAnim::Slash2:    { return "slash2";     }
            case AvatarAnim::Stab:      { return "stab";       }
            case AvatarAnim::Stab2:     { return "stab2";      }
            case AvatarAnim::Walk:      { return "walk";       }
            case AvatarAnim::WalkSlash: { return "walk-slash"; }
            case AvatarAnim::WalkSneak: { return "walk-sneak"; }
            case AvatarAnim::WalkStab:  { return "walk-stab";  }
            case AvatarAnim::WalkStab2: { return "walk-stab2"; }
            case AvatarAnim::Count:
            default:      { return "error_unkonwn_avataranim"; }
        }
        // clang-format on
    }

    [[nodiscard]] constexpr bool willLoop(const AvatarAnim t_anim) noexcept
    {
        return (
            (AvatarAnim::Idle == t_anim) or (AvatarAnim::IdleLook == t_anim) or
            (AvatarAnim::Pull == t_anim) or (AvatarAnim::Push == t_anim) or
            (AvatarAnim::Run == t_anim) or (AvatarAnim::Walk == t_anim) or
            (AvatarAnim::WalkSneak == t_anim));
    }

    [[nodiscard]] constexpr float timePerFrameSec(const AvatarAnim t_anim) noexcept
    {
        if ((AvatarAnim::Walk == t_anim) or (AvatarAnim::Run == t_anim) or
            (AvatarAnim::WalkSlash == t_anim) or (AvatarAnim::WalkSneak == t_anim) or
            (AvatarAnim::WalkStab == t_anim) or (AvatarAnim::WalkStab2 == t_anim))
        {
            return 0.03f;
        }
        else if (
            (AvatarAnim::Slash == t_anim) or (AvatarAnim::Slash2 == t_anim) or
            (AvatarAnim::Stab == t_anim) or (AvatarAnim::Stab2 == t_anim))
        {
            return 0.025f;
        }
        else if (AvatarAnim::Hurt == t_anim)
        {
            return 0.135f;
        }
        else if (AvatarAnim::Die == t_anim)
        {
            return 0.1f;
        }
        else
        {
            return 0.08f;
        }
    }

    enum class AvatarAction : unsigned char
    {
        Idle,
        Jump,
        Walk,
        Run,
        Attack,
        Hurt,
        Death
    };

    [[nodiscard]] constexpr std::string_view toString(const AvatarAction t_action) noexcept
    {
        // clang-format off
        switch(t_action)
        {
            case AvatarAction::Idle:   { return "idle";   }
            case AvatarAction::Jump:   { return "jump";   }
            case AvatarAction::Walk:   { return "walk";   }
            case AvatarAction::Run:    { return "run";    }
            case AvatarAction::Attack: { return "attack"; }
            case AvatarAction::Death:  { return "death"; }
            case AvatarAction::Hurt:   
            default:                   { return "hurt";   }
        }
        // clang-format on
    }

    struct MovementDetails
    {
        float gravity{ 0.0f };
        float walk_acc{ 0.0f };
        float walk_speed_limit{ 0.0f };
        float run_acc{ 0.0f };
        float run_speed_limit{ 0.0f };
        float jump_speed{ 0.0f };
        float jump_horiz_move_divisor{ 0.0f };
    };

    class Avatar
    {
      public:
        Avatar();

        void setup(const Context & t_context);
        void teardown() { m_animTextures.clear(); }
        void update(const Context & t_context, const float t_elapsedSec);

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        [[nodiscard]] const sf::FloatRect attackRect() const;
        void setPositionOnNewLevel(const Context & t_context, const sf::Vector2f & t_position);

      private:
        void resetAnimation(
            const Context & t_context, const AvatarAction t_action, const AvatarAnim t_anim);

        void processEnemyCollisions(const Context & t_context, const float t_elapsedSec);
        void processExitCollision(const Context & t_context);
        void preventBacktracking(const Context & t_context);
        void processCollisions(const Context & t_context);
        void processKillCollisions(const Context & t_context);
        void updateAnimation(const Context & t_context, const float t_elapsedSec);
        void afterAnimationCompletes(const Context & t_context);
        void updatePosition(const Context & t_context, const float t_elapsedSec);
        void updateJumping(const Context & t_context, const float t_elapsedSec);
        void updateAttacking(const Context & t_context);
        void updateHorizMotion(const Context & t_context, const float t_frameTimeSec);
        void scaleSprite(const Context & t_context);
        void clacMovementDetails(const Context & t_context);
        void turn();

        void collide(
            const Context & t_context,
            const sf::FloatRect & t_intersectionRect,
            const sf::Vector2f & t_avatarCenter,
            bool & t_detectLanding);

      private:
        AvatarAnim m_anim;
        AvatarAction m_action;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        sf::Vector2f m_velocity;
        bool m_isLanded;
        MovementDetails m_movement;
        bool m_isFacingRight;
        bool m_isDeathAnimComplete;
        float m_deathDelaySec;
        sf::Texture m_jumpTexture;
        std::vector<std::vector<sf::Texture>> m_animTextures;

        mutable sf::Text m_debugText;
    };
} // namespace shadowman

#endif // AVATAR_AVATAR_HPP_INCLUDED
