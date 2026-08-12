//
// info-region.cpp
//
#include "info-region.hpp"

#include "avatar/player-info.hpp"
#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

namespace shadowman
{

    InfoRegion::InfoRegion()
        : m_rect{}
        , m_heartBrightTexture{}
        , m_heartDarkTexture{}
        , m_heartSprites{}
    {}

    void InfoRegion::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_heartBrightTexture,
            (t_context.setting.media_path / "image" / "heart-bright.png"),
            true);

        util::TextureLoader::load(
            m_heartDarkTexture, (t_context.setting.media_path / "image" / "heart-dark.png"), true);
    }

    void InfoRegion::update(const Context & t_context)
    {
        m_rect = t_context.level.offscreenRect();
        m_rect.size.y *= 0.1f;

        m_heartSprites.clear();

        sf::Sprite sprite{ m_heartBrightTexture };

        const float scale{ t_context.layout.scaleBasedOnResolution(t_context, 0.25f) };
        sprite.setScale({ scale, scale });

        sf::Vector2f pos{ m_rect.position.x,
                          (util::center(m_rect).y - (sprite.getGlobalBounds().size.y * 0.5f)) };

        int brightCount{ 0 };
        for (int i{ 0 }; i < t_context.player_info.healthMax(); ++i)
        {
            if (brightCount < t_context.player_info.health())
            {
                sprite.setTexture(m_heartBrightTexture, true);
                ++brightCount;
            }
            else
            {
                sprite.setTexture(m_heartDarkTexture, true);
            }

            sprite.setPosition(pos);

            pos.x += sprite.getGlobalBounds().size.x;

            m_heartSprites.push_back(sprite);
        }
    }

    void InfoRegion::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const sf::Sprite & sprite : m_heartSprites)
        {
            t_target.draw(sprite, t_states);
        }
    }

} // namespace shadowman
