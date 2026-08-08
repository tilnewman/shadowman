//
// avatar.cpp
//
#include "avatar.hpp"

#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/filesystem-util.hpp"
#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

#include <filesystem>

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    Avatar::Avatar()
        : m_anim{ AvatarAnim::Jump }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_velocity{}
        , m_isLanded{ false }
        , m_animTextures{}
    {}

    void Avatar::setup(const Context & t_context)
    {
        // load all textures
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

        // setup sprite
        m_sprite.setTexture(m_animTextures.at(static_cast<std::size_t>(m_anim)).at(0), true);
        util::setOriginToCenter(m_sprite);

        const float scale{ t_context.setting.avatar_scale };
        m_sprite.scale({ scale, scale });

        util::centerInside(m_sprite, t_context.layout.wholeRect());
    }

    void Avatar::update(const Context & t_context, const float t_elapsedSec)
    {
        // update animation
        m_animElapsedSec += t_elapsedSec;
        const float timePerFrameSec{ 0.08f };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            if (++m_frameIndex >= m_animTextures.at(static_cast<std::size_t>(m_anim)).size())
            {
                m_frameIndex = 0;

                std::size_t animIndex{ static_cast<std::size_t>(m_anim) };
                if (++animIndex >= static_cast<std::size_t>(AvatarAnim::Count))
                {
                    animIndex = 0;
                }

                m_anim = static_cast<AvatarAnim>(animIndex);
            }

            m_sprite.setTexture(
                m_animTextures.at(static_cast<std::size_t>(m_anim)).at(m_frameIndex), true);

            util::setOriginToCenter(m_sprite);
        }

        // update position
        if (not m_isLanded)
        {
            m_velocity.y += (t_context.setting.avatar_gravity * t_elapsedSec);
        }

        // const sf::Vector2f posBefore{ m_sprite.getPosition() };
        m_sprite.move(m_velocity);
        processCollisions(t_context);
        // const sf::Vector2f posAfter{ m_sprite.getPosition() };
    }

    void Avatar::resetAnimation(const AvatarAnim t_anim)
    {
        m_anim           = t_anim;
        m_animElapsedSec = 0.0f;
        m_frameIndex     = 0;
        m_velocity       = { 0.0f, 0.0f };
        m_isLanded       = false;

        m_sprite.setTexture(
            m_animTextures.at(static_cast<std::size_t>(m_anim)).at(m_frameIndex), true);

        util::setOriginToCenter(m_sprite);

    }

    void Avatar::draw(
        const sf::Vector2f & t_mapToOffscreenOffset,
        sf::RenderTarget & t_target,
        sf::RenderStates t_states) const
    {
        sf::Sprite tempAvatarSprite{ m_sprite };
        tempAvatarSprite.move(t_mapToOffscreenOffset);
        t_target.draw(tempAvatarSprite, t_states);
    }

    const sf::FloatRect Avatar::collisionRect() const
    {
        return util::scaleRectInPlaceCopy(m_sprite.getGlobalBounds(), 0.8f);
    }

    void Avatar::setPositionOnNewLevel(const sf::Vector2f & t_position)
    {
        resetAnimation(AvatarAnim::Idle);
        m_sprite.setPosition(t_position);
    }

    void Avatar::processCollisions(const Context & t_context)
    {
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

        if (!detectLanding)
        {
            m_isLanded = false;
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

        if ((m_velocity.y < 0.0f) && (t_intersectionRect.size.y < tolerance) &&
            (intersectCenter.y < t_avatarCenter.y))
        {
            // rising and hit something abov
            m_velocity.y = 0.0f;
            m_sprite.move({ 0.0f, t_intersectionRect.size.y });
        }
        else if (
            (m_velocity.y > 0.0f) && (t_intersectionRect.size.y < tolerance) &&
            (intersectCenter.y > t_avatarCenter.y))
        {
            // falling and hit something below

            if (!m_isLanded)
            {
                // TODO t_context.audio.play("land");
                // m_action = Action::Idle;
                resetAnimation(AvatarAnim::Idle);
            }

            m_isLanded      = true;
            t_detectLanding = true;
            m_velocity.y    = 0.0f;
            m_sprite.move({ 0.0f, -t_intersectionRect.size.y });
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

} // namespace shadowman
