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

    class Fly
    {
      public:
       
        void update(const Context & t_context, const float t_elapsedSec);
        
        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const;


      private:
    };

} // namespace shadowman

#endif // ENEMY_FLY_HPP_INCLUDED
