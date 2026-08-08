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

      private:
        AvatarAnim m_anim;
        sf::Sprite m_sprite;
        float m_animElapsedSec;
        std::size_t m_frameIndex;
        std::vector<std::vector<sf::Texture>> m_animTextures;
    };
} // namespace shadowman

#endif // AVATAR_AVATAR_HPP_INCLUDED
