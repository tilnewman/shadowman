//
// fly.cpp
//
#include "fly.hpp"

#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/random.hpp"
#include "util/sfml-util.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{
    Fly::Fly(
        const Context & t_context,
        const FlyType t_type,
        const FlyTextures & t_textures,
        const sf::FloatRect & t_rect)
        : m_type{ t_type }
        , m_rect{ t_rect }
        , m_sprite{ t_textures.fly.at(0) }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_textures{ t_textures }
    {
        const float scale{ t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.fly_scale) };

        m_sprite.setScale({ scale, scale });

        if (m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
        }

        m_sprite.setPosition(
            { util::center(m_rect).x,
              (util::center(m_rect).y - (m_sprite.getGlobalBounds().size.y * 0.5f)) });
    }

    void Fly::update(const Context &, const float) {}

    void Fly::draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        // TODO skip draw if no visible

        sf::Sprite tempSprite{ m_sprite };
        tempSprite.move(t_context.level.mapToOffscreenOffset());
        t_target.draw(tempSprite, t_states);
    }

} // namespace shadowman