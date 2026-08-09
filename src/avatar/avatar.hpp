#ifndef AVATAR_AVATAR_HPP_INCLUDED
#define AVATAR_AVATAR_HPP_INCLUDED
//
// avatar.hpp
//
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

    enum class AvatarAnim : unsigned char
    {
        Die = 0,
        Idle,
        IdleLook,
        Jump,
        Kick,
        Pull,
        Push,
        Run,
        Slash,
        SlashEffect,
        Slash2,
        Slash2Effect,
        Slide,
        Stab,
        Stab2,
        Stab2Effect,
        Walk,
        WalkSlash,
        WalkSneek,
        WalkStab,
        WalkStab2,
        Count
    };

    [[nodiscard]] constexpr std::string_view toString(const AvatarAnim t_anim) noexcept
    {
        // clang-format off
        switch(t_anim)
        {
            case AvatarAnim::Die:           { return "die";           }
            case AvatarAnim::Idle:          { return "idle";          }
            case AvatarAnim::IdleLook:      { return "idle-look";     }
            case AvatarAnim::Jump:          { return "jump";          }
            case AvatarAnim::Kick:          { return "kick";          }
            case AvatarAnim::Pull:          { return "pull";          }
            case AvatarAnim::Push:          { return "push";          }
            case AvatarAnim::Run:           { return "run";           }
            case AvatarAnim::Slash:         { return "slash";         }
            case AvatarAnim::SlashEffect:   { return "slash-effect";  }
            case AvatarAnim::Slash2:        { return "slash2";        }
            case AvatarAnim::Slash2Effect:  { return "slash2-effect"; }
            case AvatarAnim::Slide:         { return "slide";         }
            case AvatarAnim::Stab:          { return "stab";          }
            case AvatarAnim::Stab2:         { return "stab2";         }
            case AvatarAnim::Stab2Effect:   { return "stab2-effect";  }
            case AvatarAnim::Walk:          { return "walk";          }
            case AvatarAnim::WalkSlash:     { return "walk-slash";    }
            case AvatarAnim::WalkSneek:     { return "walk-sneek";    }
            case AvatarAnim::WalkStab:      { return "walk-stab";     }
            case AvatarAnim::WalkStab2:     { return "walk-stab2";    }
            case AvatarAnim::Count:
            default:             { return "error_unkonwn_avataranim"; }
        }
        // clang-format on
    }

    [[nodiscard]] constexpr bool willLoop(const AvatarAnim t_anim) noexcept
    {
        return (
            (AvatarAnim::Idle == t_anim) or (AvatarAnim::IdleLook == t_anim) or
            (AvatarAnim::Pull == t_anim) or (AvatarAnim::Push == t_anim) or
            (AvatarAnim::Run == t_anim) or (AvatarAnim::Slide == t_anim) or
            (AvatarAnim::Walk == t_anim) or (AvatarAnim::WalkSneek == t_anim));
    }

    enum class AvatarAction : unsigned char
    {
        Idle,
        Jump,
        Walk,
        Run,
        Attack
    };

    class Avatar
    {
      public:
        Avatar();

        void setup(const Context & t_context);
        void teardown() { m_animTextures.clear(); }
        void update(const Context & t_context, const float t_elapsedSec);

        void draw(
            const sf::Vector2f & t_mapToOffscreenOffset,
            sf::RenderTarget & t_target,
            sf::RenderStates t_states) const;

        [[nodiscard]] const sf::FloatRect collisionRect() const;
        void setPositionOnNewLevel(const Context & t_context, const sf::Vector2f & t_position);

      private:
        void resetAnimation(const Context & t_context, const AvatarAnim t_anim);
        void processCollisions(const Context & t_context);
        void updateAnimation(const Context & t_context, const float t_elapsedSec);
        void updatePosition(const Context & t_context, const float t_elapsedSec);
        void updateJumping(const Context & t_context, const float t_elapsedSec);

        void scaleSprite(const Context & t_context);

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
        sf::Texture m_jumpTexture;
        std::vector<std::vector<sf::Texture>> m_animTextures;
    };
} // namespace shadowman

#endif // AVATAR_AVATAR_HPP_INCLUDED
