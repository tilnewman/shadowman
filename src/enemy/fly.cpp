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
        , m_task{ FlyTask::Idle }
        , m_isDying{ false }
        , m_rect{ t_rect }
        , m_sprite{ t_textures.fly.at(0) }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_frameIndex{ 0 }
        , m_idleElapsedSec{ 0.0f }
        , m_idleDurationSec{ 0.0f }
        , m_wanderTarget{ 0.0f }
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

    void Fly::turn()
    {
        m_isFacingRight = not m_isFacingRight;
        m_sprite.scale({ -1.0f, 1.0f });
    }

    void Fly::updateAnimation(const float t_elapsedSec)
    {
        m_animElapsedSec += t_elapsedSec;
        const float timeBetweenFramees{ timePerFrameSec(m_task) };
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

    void Fly::update(const Context & t_context, const float t_elapsedSec)
    {
        updateAnimation(t_elapsedSec);

        if (FlyTask::Idle == m_task)
        {
            m_idleElapsedSec += t_elapsedSec;
            if (m_idleElapsedSec > m_idleDurationSec)
            {
                m_idleElapsedSec = 0.0f;

                if (t_context.random.boolean())
                {
                    turn();
                }
                else
                {
                    startWandering(t_context);
                }
            }
        }
        else if (FlyTask::Wander == m_task)
        {
            const float wanderSpeed{ 30.0f * ((m_isFacingRight) ? 1.0f : -1.0f) };
            m_sprite.move({ (wanderSpeed * t_elapsedSec), 0.0f });

            const float distanceToTarget{ std::abs(m_wanderTarget - util::center(m_sprite).x) };
            if (distanceToTarget < 5.0f)
            {
                if (t_context.random.boolean())
                {
                    startIdling(t_context);
                }
                else
                {
                    startWandering(t_context);
                }
            }
        }
    }

    void Fly::startIdling(const Context& t_context)
    {
        m_task = FlyTask::Idle;
        m_idleElapsedSec  = 0.0f;
        m_idleDurationSec = t_context.random.fromTo(1.5f, 4.0f);
    }

    void Fly::startWandering(const Context & t_context)
    {
        m_task = FlyTask::Wander;

        const float halfWidth{ m_sprite.getGlobalBounds().size.x * 0.5f };

        m_wanderTarget = t_context.random.fromTo(
            (m_rect.position.x + halfWidth), (util::right(m_rect) - halfWidth));

        const bool isTargetRight{ util::center(m_sprite).x < m_wanderTarget };
        if (isTargetRight != m_isFacingRight)
        {
            turn();
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