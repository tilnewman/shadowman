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
        , m_isDying{ false }
        , m_rect{ t_rect }
        , m_sprite{ t_textures.fly.at(0) }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_textures{ t_textures }
    {
        util::setOriginToCenter(m_sprite);

        const float scale{ t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.fly_scale) };

        m_sprite.setScale({ scale, scale });

        if (m_isFacingRight)
        {
            m_sprite.scale({ -1.0f, 1.0f });
        }

        m_sprite.setPosition(util::center(m_rect));
    }

    void Fly::update(const Context &, const float t_elapsedSec)
    {
        m_animElapsedSec += t_elapsedSec;
        const float timeBetweenFramees{ (m_isDying) ? 0.125f : 0.055f };
        if (m_animElapsedSec > timeBetweenFramees)
        {
            m_animElapsedSec -= timeBetweenFramees;

            const std::vector<sf::Texture> & textures{ (m_isDying) ? m_textures.die
                                                                   : m_textures.fly };

            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;
            }

            m_sprite.setTexture(textures.at(m_frameIndex));
        }
    }

    void Fly::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        // TODO skip draw if not visible

        sf::Sprite tempSprite{ m_sprite };
        tempSprite.move(t_context.level.mapToOffscreenOffset());
        t_target.draw(tempSprite, t_states);
    }

} // namespace shadowman