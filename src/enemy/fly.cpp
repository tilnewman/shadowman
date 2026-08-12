//
// fly.cpp
//
#include "fly.hpp"

#include "avatar/avatar.hpp"
#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/random.hpp"
#include "util/sfml-util.hpp"
#include "util/sound-player.hpp"

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
        , m_rect{ t_rect }
        , m_sprite{ t_textures.fly.at(0) }
        , m_isFacingRight{ t_context.random.boolean() }
        , m_frameIndex{ 0 }
        , m_idleElapsedSec{ 0.0f }
        , m_idleDurationSec{ 0.0f }
        , m_wanderTarget{ 0.0f }
        , m_isAlive{ true }
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

    const sf::FloatRect Fly::collisionRect() const
    {
        sf::FloatRect rect{ m_sprite.getGlobalBounds() };
        util::scaleRectInPlace(rect, { 0.35f, 0.65f });

        if (m_isFacingRight)
        {
            rect.size.x *= 1.15f;
        }
        else
        {
            const float horizOffset{ rect.size.x * 0.15f };
            rect.position.x -= horizOffset;
            rect.size.x += (horizOffset * 1.4f);
        }

        return rect;
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

            const std::vector<sf::Texture> & textures{ (FlyTask::Death == m_task)
                                                           ? m_textures.die
                                                           : m_textures.fly };

            if (++m_frameIndex >= textures.size())
            {
                m_frameIndex = 0;

                if (FlyTask::Death == m_task)
                {
                    m_isAlive = false;
                }
            }

            m_sprite.setTexture(textures.at(m_frameIndex), true);
        }
    }

    void Fly::update(const Context & t_context, const float t_elapsedSec)
    {
        updateAnimation(t_elapsedSec);

        if ((FlyTask::Idle == m_task) or (FlyTask::Wander == m_task))
        {
            if (t_context.avatar.collisionRect().findIntersection(m_rect))
            {
                startChasing(t_context);
            }
        }

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
        else if (FlyTask::Chase == m_task)
        {
            turnToward(util::center(t_context.avatar.collisionRect()).x);

            const float chaseSpeed{ 70.0f * ((m_isFacingRight) ? 1.0f : -1.0f) };
            const sf::Vector2f move{ (chaseSpeed * t_elapsedSec), 0.0f };
            m_sprite.move(move);

            const sf::FloatRect collRect{ collisionRect() };
            if ((collRect.position.x < m_rect.position.x) or
                (util::right(collRect) > util::right(m_rect)))
            {
                m_sprite.move(-1.0f * move);
            }

            const sf::FloatRect playerRect{ t_context.avatar.collisionRect() };
            if (not playerRect.findIntersection(m_rect))
            {
                startIdling(t_context);
            }
        }
    }

    void Fly::startChasing(const Context & t_context)
    {
        m_task = FlyTask::Chase;
        t_context.audio.play("fly-notice");
        turnToward(util::center(t_context.avatar.collisionRect()).x);
    }

    void Fly::startIdling(const Context & t_context)
    {
        m_task            = FlyTask::Idle;
        m_idleElapsedSec  = 0.0f;
        m_idleDurationSec = t_context.random.fromTo(1.5f, 4.0f);
    }

    void Fly::startWandering(const Context & t_context)
    {
        m_task = FlyTask::Wander;

        const float halfWidth{ m_sprite.getGlobalBounds().size.x * 0.5f };

        m_wanderTarget = t_context.random.fromTo(
            (m_rect.position.x + halfWidth), (util::right(m_rect) - halfWidth));

        turnToward(m_wanderTarget);
    }

    void Fly::turnToward(const float t_posHoriz)
    {
        const bool isTargetRight{ util::center(m_sprite).x < t_posHoriz };
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

    void Fly::kill(const Context & t_context)
    {
        if (isAlive() and (FlyTask::Death != m_task))
        {
            m_task           = FlyTask::Death;
            m_animElapsedSec = 0.0f;
            m_frameIndex     = 0;
            t_context.audio.play("fly-death");
        }
    }

} // namespace shadowman