//
// pickup.cpp
//
#include "pickup.hpp"

#include "avatar/player-info.hpp"
#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/check-macros.hpp"
#include "util/sfml-util.hpp"
#include "util/sound-player.hpp"
#include "util/texture-loader.hpp"

namespace shadowman
{

    FlareAnim::FlareAnim(const sf::Sprite & t_sprite)
        : sprite(t_sprite)
    {}

    //

    PickupAnim::PickupAnim(
        const Context & t_context,
        const Pickup t_type,
        const sf::Texture & t_texture,
        const sf::Vector2f & t_position)
        : m_type{ t_type }
        , m_sprite{ t_texture }
        , m_animElpasedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_isAlive{ true }
    {
        m_sprite.setTextureRect(textureRect(m_frameIndex));
        const float scale{ t_context.layout.scaleBasedOnResolution(t_context, 2.0f) };
        m_sprite.setScale({ scale, scale });
        util::setOriginToCenter(m_sprite);
        m_sprite.setPosition(t_position);
    }

    const sf::IntRect PickupAnim::textureRect(const std::size_t t_frameIndex) const
    {
        sf::IntRect rect;
        rect.size.x     = static_cast<int>(m_sprite.getTexture().getSize().y);
        rect.size.y     = rect.size.x;
        rect.position.y = 0;
        rect.position.x = (rect.size.x * static_cast<int>(t_frameIndex));
        return rect;
    }

    std::size_t PickupAnim::frameCount() const
    {
        return static_cast<std::size_t>(
            m_sprite.getTexture().getSize().x / m_sprite.getTexture().getSize().y);
    }

    void PickupAnim::update(const Context &, const float t_elapsedSec)
    {
        m_animElpasedSec += t_elapsedSec;
        const float timePerFrameSec{ 0.1f };
        if (m_animElpasedSec > timePerFrameSec)
        {
            m_animElpasedSec -= timePerFrameSec;

            if (++m_frameIndex >= frameCount())
            {
                m_frameIndex = 0;
            }

            m_sprite.setTextureRect(textureRect(m_frameIndex));
        }
    }

    void PickupAnim::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        // TODO check if onscreen before drawing

        sf::Sprite tempSprite{ m_sprite };
        tempSprite.move(t_context.level.mapToOffscreenOffset());
        t_target.draw(tempSprite, t_states);
    }

    const sf::Sprite PickupAnim::makeFlareSprite() const
    {
        sf::Sprite sprite{ m_sprite };
        sprite.setTextureRect(textureRect(0));
        return sprite;
    }

    //

    PickupManager::PickupManager()
        : m_heartTexture{}
    {}

    void PickupManager::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_heartTexture,
            (t_context.setting.media_path / "image" / "heart" / "heart-pickup.png"),
            true);
    }

    void PickupManager::update(const Context & t_context, const float t_elapsedSec)
    {
        for (PickupAnim & anim : m_anims)
        {
            anim.update(t_context, t_elapsedSec);
        }

        for (FlareAnim & anim : m_flareAnims)
        {
            const float scaler{ 1.07f };
            anim.sprite.scale({ scaler, scaler });

            std::uint8_t alpha{ anim.sprite.getColor().a };
            if (alpha > 10u)
            {
                alpha -= 8;
            }
            sf::Color newColor{ 255u, 255u, 255u, alpha };
            anim.sprite.setColor(newColor);
        }

        std::erase_if(m_flareAnims, [](const FlareAnim & anim) { return not anim.isAlive(); });
    }

    void PickupManager::add(
        const Context & t_context, const std::string & t_name, const sf::FloatRect & t_rect)
    {
        const Pickup type{ fromString(t_name) };
        M_CHECK((type != Pickup::Count), "Unknown pickup type name \"" << t_name << "\"");

        if (Pickup::Heart == type)
        {
            m_anims.emplace_back(t_context, type, m_heartTexture, util::center(t_rect));
        }
    }

    void PickupManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const PickupAnim & anim : m_anims)
        {
            anim.draw(t_context, t_target, t_states);
        }

        for (const FlareAnim & anim : m_flareAnims)
        {
            sf::Sprite tempSprite{ anim.sprite };
            tempSprite.move(t_context.level.mapToOffscreenOffset());
            t_target.draw(tempSprite, t_states);
        }
    }

    void PickupManager::playerPickup(const Context & t_context, const sf::FloatRect & t_playerRect)
    {
        for (PickupAnim & anim : m_anims)
        {
            if ((t_context.player_info.health() < t_context.player_info.healthMax()) and
                t_playerRect.findIntersection(anim.collisionRect()))
            {
                if (Pickup::Heart == anim.type())
                {
                    t_context.player_info.healthAdjust(1);
                }

                anim.kill();
                t_context.audio.play("pickup");
                m_flareAnims.emplace_back(anim.makeFlareSprite());
            }
        }

        std::erase_if(m_anims, [](const PickupAnim & anim) { return not anim.isAlive(); });
    }

} // namespace shadowman
