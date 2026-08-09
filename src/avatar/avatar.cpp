//
// avatar.cpp
//
#include "avatar.hpp"

#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/filesystem-util.hpp"
#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"
#include "util/sound-player.hpp"
#include "util/texture-loader.hpp"

#include <filesystem>

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    Avatar::Avatar()
        : m_anim{ AvatarAnim::Idle }
        , m_action{ AvatarAction::Idle }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_velocity{}
        , m_isLanded{ false }
        , m_movement{}
        , m_isFacingRight{ true }
        , m_isDeathAnimComplete{ false }
        , m_jumpTexture{}
        , m_animTextures{}
        , m_debugText{ util::SfmlDefaults::instance().font() }
    {}

    void Avatar::setup(const Context & t_context)
    {
        const std::size_t animCount{ static_cast<std::size_t>(AvatarAnim::Count) };
        m_animTextures.reserve(animCount);

        for (std::size_t animIndex{ 0 }; animIndex < animCount; ++animIndex)
        {
            const AvatarAnim anim{ static_cast<AvatarAnim>(animIndex) };
            std::vector<sf::Texture> & textures{ m_animTextures.emplace_back() };

            const auto dirPath{ t_context.setting.media_path / "image" / "avatar" /
                                toString(anim) };

            const auto imagePaths{ util::findFilesInDirectory(dirPath, ".png") };

            M_CHECK(not imagePaths.empty(), "Failed to find any PNG image files in: " << dirPath);

            for (const auto & path : imagePaths)
            {
                util::TextureLoader::load(textures.emplace_back(), path, true);
            }
        }

        util::TextureLoader::load(
            m_jumpTexture,
            (t_context.setting.media_path / "image" / "avatar" / "jump" / "jump-10.png"),
            true);

        resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
        clacMovementDetails(t_context);

        m_debugText = t_context.font.makeText(Font::General, FontSize::Small, "", sf::Color::Black);
    }

    void Avatar::turn()
    {
        m_isFacingRight = not m_isFacingRight;
        m_sprite.scale({ -1.0f, 1.0f });
    }

    void Avatar::scaleSprite(const Context & t_context)
    {
        float scale{ t_context.setting.avatar_scale };
        if (AvatarAnim::Jump == m_anim)
        {
            scale *= 1.28f;
        }
        else if (AvatarAnim::Slash == m_anim)
        {
            scale *= 1.1f;
        }
        else if (AvatarAnim::Slash2 == m_anim)
        {
            scale *= 1.2f;
        }
        else if (AvatarAnim::Die == m_anim)
        {
            scale *= 1.3f;
        }

        m_sprite.setScale({ ((m_sprite.getScale().x < 0.0f) ? -scale : scale), scale });
    }

    void Avatar::update(const Context & t_context, const float t_elapsedSec)
    {
        if (m_isDeathAnimComplete)
        {
            return;
        }

        const sf::Vector2f beforePos{ m_sprite.getPosition() };

        updateJumping(t_context, t_elapsedSec);
        updateAttacking(t_context);
        updateHorizMotion(t_context, t_elapsedSec);
        updateAnimation(t_context, t_elapsedSec);
        updatePosition(t_context, t_elapsedSec);
        processCollisions(t_context);
        processKillCollisions(t_context);

        const sf::Vector2f afterPos{ m_sprite.getPosition() };
        t_context.level.playerMove(t_context, collisionRect(), (afterPos - beforePos));
    }

    void Avatar::updateJumping(const Context & t_context, const float t_elapsedSec)
    {
        if (m_isLanded and sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) and
            ((AvatarAction::Idle == m_action) or (AvatarAction::Walk == m_action) or
             (AvatarAction::Run == m_action)))
        {
            m_isLanded = false;
            m_velocity.y -= (m_movement.jump_speed * t_elapsedSec);
            resetAnimation(t_context, AvatarAction::Jump, AvatarAnim::Jump);
            t_context.audio.stop("walk");
            t_context.audio.play("jump");
        }
    }

    void Avatar::updateAttacking(const Context & t_context)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::F) and
            (AvatarAction::Attack != m_action) and (AvatarAction::Death != m_action) and
            (AvatarAction::Hurt != m_action))
        {
            const AvatarAnim randomAnim{ t_context.random.from(
                { AvatarAnim::Stab, AvatarAnim::Stab2, AvatarAnim::Slash, AvatarAnim::Slash2 }) };

            resetAnimation(t_context, AvatarAction::Attack, randomAnim);
            t_context.audio.play("swipe");
        }
    }

    void Avatar::updateAnimation(const Context & t_context, const float t_elapsedSec)
    {
        if ((AvatarAction::Jump == m_action) or m_isDeathAnimComplete)
        {
            return;
        }

        m_animElapsedSec += t_elapsedSec;
        const float frameTimeSec{ timePerFrameSec(m_anim) };
        if (m_animElapsedSec > frameTimeSec)
        {
            m_animElapsedSec -= frameTimeSec;

            if (++m_frameIndex >= m_animTextures.at(static_cast<std::size_t>(m_anim)).size())
            {
                afterAnimationCompletes(t_context);
            }

            m_sprite.setTexture(
                m_animTextures.at(static_cast<std::size_t>(m_anim)).at(m_frameIndex), true);

            util::setOriginToCenter(m_sprite);
        }
    }

    void Avatar::afterAnimationCompletes(const Context & t_context)
    {
        if (willLoop(m_anim))
        {
            m_frameIndex = 0;

            if ((AvatarAction::Idle == m_action) and (AvatarAnim::Idle == m_anim) and
                (t_context.random.fromTo(1, 100) < 25))
            {
                resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::IdleLook);
            }
            else if ((AvatarAction::Idle == m_action) and (AvatarAnim::IdleLook == m_anim))
            {
                resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
            }
        }
        else if (AvatarAction::Death == m_action)
        {
            m_isDeathAnimComplete = true;
            m_frameIndex          = 0;
            setPositionOnNewLevel(t_context, util::center(t_context.level.enterRect()));
        }
        else
        {
            resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
        }
    }

    void Avatar::updatePosition(const Context &, const float t_elapsedSec)
    {
        m_velocity.y += (m_movement.gravity * t_elapsedSec);
        m_sprite.move(m_velocity);
    }

    void Avatar::resetAnimation(
        const Context & t_context, const AvatarAction t_action, const AvatarAnim t_anim)
    {
        m_action         = t_action;
        m_anim           = t_anim;
        m_animElapsedSec = 0.0f;
        m_frameIndex     = 0;

        if (AvatarAnim::Jump == m_anim)
        {
            m_sprite.setTexture(m_jumpTexture, true);
        }
        else
        {
            m_sprite.setTexture(
                m_animTextures.at(static_cast<std::size_t>(m_anim)).at(m_frameIndex), true);
        }

        util::setOriginToCenter(m_sprite);
        scaleSprite(t_context);
    }

    void Avatar::draw(
        const sf::Vector2f & t_mapToOffscreenOffset,
        sf::RenderTarget & t_target,
        sf::RenderStates t_states) const
    {
        sf::Sprite tempSprite{ m_sprite };
        tempSprite.move(t_mapToOffscreenOffset);
        t_target.draw(tempSprite, t_states);

        // sf::FloatRect collRect{ collisionRect() };
        // collRect.position += t_mapToOffscreenOffset;
        // util::drawRectangleShape(t_target, collRect, false, sf::Color::Red);

        // std::string str{ toString(m_action) };
        // str += ", ";
        // str += toString(m_anim);
        // str += ", ";
        // str += ((m_isDeathAnimComplete) ? "true" : "false");
        // m_debugText.setString(str);
        // util::setOriginToPosition(m_debugText);
        // m_debugText.setPosition({ util::right(tempSprite), tempSprite.getPosition().y });
        // t_target.draw(m_debugText, t_states);
    }

    const sf::FloatRect Avatar::collisionRect() const
    {
        sf::FloatRect rect{ util::scaleRectInPlaceCopy(
            m_sprite.getGlobalBounds(), { 0.35f, 0.8f }) };

        rect.size.y *= 1.125f;

        if (AvatarAnim::Jump == m_anim)
        {
            rect.size.y *= 0.75f;
        }
        else if ((AvatarAnim::Stab == m_anim) or (AvatarAnim::Stab2 == m_anim))
        {
            if (m_isFacingRight)
            {
                rect.size.x *= 0.5f;
            }
            else
            {
                rect.position.x += (rect.size.x * 0.35f);
                rect.size.x *= 0.8f;
            }
        }
        else if (AvatarAnim::IdleLook == m_anim)
        {
            rect.size.x *= 0.8f;

            if (not m_isFacingRight)
            {
                rect.position.x += (rect.size.x * 0.25f);
            }
        }
        else if (AvatarAnim::Slash == m_anim)
        {
            const float horizOffset{ rect.size.x * 0.25f };
            rect.position.x += horizOffset;
            rect.size.x -= horizOffset;

            if (not m_isFacingRight)
            {
                rect.size.x *= 0.75f;
            }
        }
        else if (AvatarAnim::Slash2 == m_anim)
        {
            rect.size.x *= 0.75f;

            const float vertOffset{ rect.size.y * 0.25f };
            rect.position.y += vertOffset;
            rect.size.y -= vertOffset;

            if (not m_isFacingRight)
            {
                rect.position.x += (rect.size.x * 0.3f);
            }
        }

        return rect;
    }

    void Avatar::setPositionOnNewLevel(const Context & t_context, const sf::Vector2f & t_position)
    {
        m_velocity            = { 0.0f, 0.0f };
        m_isLanded            = false;
        m_isDeathAnimComplete = false;
        resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
        m_sprite.setPosition(t_position);
        m_sprite.setColor(sf::Color::White);

        if (not m_isFacingRight)
        {
            turn();
        }
    }

    void Avatar::processKillCollisions(const Context & t_context)
    {
        if (AvatarAction::Death == m_action)
        {
            return;
        }

        const sf::FloatRect avatarRect{ collisionRect() };
        for (const sf::FloatRect & killRect : t_context.level.killCollisions())
        {
            if (avatarRect.findIntersection(killRect))
            {
                m_velocity = { 0.0f, 0.0f };
                resetAnimation(t_context, AvatarAction::Death, AvatarAnim::Die);
                m_sprite.setColor(sf::Color::Red);
                turn();
                return;
            }
        }
    }

    void Avatar::processCollisions(const Context & t_context)
    {
        if (AvatarAction::Death == m_action)
        {
            return;
        }

        const sf::FloatRect avatarRect{ collisionRect() };
        const sf::Vector2f avatarCenter{ util::center(avatarRect) };

        bool detectLanding{ false };

        for (const sf::FloatRect & collRect : t_context.level.collisions())
        {
            const auto intersectionOpt{ avatarRect.findIntersection(collRect) };
            if (intersectionOpt)
            {
                collide(t_context, intersectionOpt.value(), avatarCenter, detectLanding);
            }
        }

        if (not detectLanding)
        {
            m_isLanded = false;
            t_context.audio.stop("walk");
        }
    }

    void Avatar::collide(
        const Context & t_context,
        const sf::FloatRect & t_intersectionRect,
        const sf::Vector2f & t_avatarCenter,
        bool & t_detectLanding)
    {
        const float tolerance{ 25.0f }; // this magic number brought to you by zTn 2021-8-2
        const sf::Vector2f intersectCenter{ util::center(t_intersectionRect) };

        if ((m_velocity.y < 0.0f) and (t_intersectionRect.size.y < tolerance) and
            (intersectCenter.y < t_avatarCenter.y))
        {
            // rising and hit something abov
            m_velocity.y = 0.0f;
            m_sprite.move({ 0.0f, t_intersectionRect.size.y });
        }
        else if (
            (m_velocity.y > 0.0f) and (t_intersectionRect.size.y < tolerance) and
            (intersectCenter.y > t_avatarCenter.y))
        {
            // falling and hit something below

            if (not m_isLanded)
            {
                t_context.audio.play("land");
                resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
            }

            m_isLanded      = true;
            t_detectLanding = true;
            m_velocity.y    = 0.0f;
            m_sprite.move({ 0.0f, -t_intersectionRect.size.y });

            if (not sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) and
                not sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            {
                m_velocity.x = 0.0f;
            }
        }
        else if (t_intersectionRect.size.x < tolerance)
        {
            // hit something from the side
            m_velocity.x = 0.0f;

            if (intersectCenter.x < t_avatarCenter.x)
            {
                m_sprite.move({ t_intersectionRect.size.x, 0.0f });
            }
            else
            {
                m_sprite.move({ -t_intersectionRect.size.x, 0.0f });
            }
        }
    }

    void Avatar::updateHorizMotion(const Context & t_context, const float t_frameTimeSec)
    {
        if ((AvatarAction::Death == m_action) or (AvatarAction::Hurt == m_action) or
            (AvatarAction::Attack == m_action) or
            sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
        {
            return;
        }

        const auto enforceSpeedLimitWalk = [&]() {
            if (m_velocity.x > m_movement.walk_speed_limit)
            {
                m_velocity.x = m_movement.walk_speed_limit;
            }
            else if (m_velocity.x < -m_movement.walk_speed_limit)
            {
                m_velocity.x = -m_movement.walk_speed_limit;
            }
        };

        const auto enforceSpeedLimitRun = [&]() {
            if (m_velocity.x > m_movement.run_speed_limit)
            {
                m_velocity.x = m_movement.run_speed_limit;
            }
            else if (m_velocity.x < -m_movement.run_speed_limit)
            {
                m_velocity.x = -m_movement.run_speed_limit;
            }
        };

        if (AvatarAction::Jump == m_action)
        {
            // Allow moving side-to-side at a reduced rate while in the air.
            // It sounds wrong but feels so right. What the hell, mario did it.
            const float jumpMoveDivisor{ m_movement.jump_horiz_move_divisor };

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
            {
                m_velocity.x += ((m_movement.walk_acc / jumpMoveDivisor) * t_frameTimeSec);
                enforceSpeedLimitWalk();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
            {
                m_velocity.x -= ((m_movement.walk_acc / jumpMoveDivisor) * t_frameTimeSec);
                enforceSpeedLimitWalk();
            }

            return;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift))
            {
                m_velocity.x += (m_movement.run_acc * t_frameTimeSec);
                enforceSpeedLimitRun();

                if (AvatarAction::Run != m_action)
                {
                    t_context.audio.play("walk");
                    resetAnimation(t_context, AvatarAction::Run, AvatarAnim::Run);
                }
            }
            else
            {
                m_velocity.x += (m_movement.walk_acc * t_frameTimeSec);
                enforceSpeedLimitWalk();

                if (AvatarAction::Walk != m_action)
                {
                    t_context.audio.play("walk");

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                    {
                        resetAnimation(t_context, AvatarAction::Walk, AvatarAnim::WalkSneak);
                    }
                    else
                    {
                        resetAnimation(t_context, AvatarAction::Walk, AvatarAnim::Walk);
                    }
                }
            }

            if (!m_isFacingRight)
            {
                turn();
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::LShift))
            {
                m_velocity.x -= (m_movement.run_acc * t_frameTimeSec);
                enforceSpeedLimitRun();

                if (AvatarAction::Run != m_action)
                {
                    t_context.audio.play("walk");
                    resetAnimation(t_context, AvatarAction::Run, AvatarAnim::Run);
                }
            }
            else
            {
                m_velocity.x -= (m_movement.walk_acc * t_frameTimeSec);
                enforceSpeedLimitWalk();

                if (AvatarAction::Walk != m_action)
                {
                    t_context.audio.play("walk");

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                    {
                        resetAnimation(t_context, AvatarAction::Walk, AvatarAnim::WalkSneak);
                    }
                    else
                    {
                        resetAnimation(t_context, AvatarAction::Walk, AvatarAnim::Walk);
                    }
                }
            }

            if (m_isFacingRight)
            {
                turn();
            }
        }
        else
        {
            if (AvatarAction::Idle != m_action)
            {
                m_velocity.x = 0.0f;
                t_context.audio.stop("walk");
                resetAnimation(t_context, AvatarAction::Idle, AvatarAnim::Idle);
            }
        }
    }

    void Avatar::clacMovementDetails(const Context & t_context)
    {
        m_movement.gravity =
            t_context.layout.scaleBasedOnResolution(t_context, t_context.setting.avatar_gravity);

        m_movement.walk_acc =
            t_context.layout.scaleBasedOnResolution(t_context, t_context.setting.avatar_walk_acc);

        m_movement.walk_speed_limit = t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.avatar_walk_speed_limit);

        m_movement.run_acc =
            t_context.layout.scaleBasedOnResolution(t_context, t_context.setting.avatar_run_acc);

        m_movement.run_speed_limit = t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.avatar_run_speed_limit);

        m_movement.jump_speed =
            t_context.layout.scaleBasedOnResolution(t_context, t_context.setting.avatar_jump_speed);

        m_movement.jump_horiz_move_divisor = t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.avatar_jump_horiz_move_divisor);
    }

} // namespace shadowman
