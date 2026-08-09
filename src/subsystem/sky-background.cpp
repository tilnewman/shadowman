//
// sky-background.cpp
//
#include "sky-background.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/random.hpp"
#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    CloudAnim::CloudAnim()
        : sprite{ util::SfmlDefaults::instance().texture() }
        , speed{ 0.0f }
    {}

    //

    SkyBackground::SkyBackground()
        : m_offscreenRect{}
        , m_skyVerts{}
        , m_cloud1Texture{}
        , m_cloud2Texture{}
        , m_cloud3Texture{}
        , m_moonTexture{}
        , m_moonSprite{ m_moonTexture }
        , m_sunTexture{}
        , m_sunSprite{ m_sunTexture }
        , m_cloudAnims{}
        , m_willShowMoon{ false }
        , m_willShowSun{ true }
        , m_skyColors{
            { .top = sf::Color(254, 204, 0), .bot = sf::Color(136, 136, 136) },
            { .top = sf::Color(254, 254, 204), .bot = sf::Color(56, 106, 106) },
            { .top = sf::Color(151, 183, 5), .bot = sf::Color(56, 106, 106) },
            { .top = sf::Color(255, 203, 253), .bot = sf::Color(205, 105, 6) },
        }
    {}

    void SkyBackground::setup(const Context & t_context, const sf::Vector2f & t_size)
    {
        m_offscreenRect.position = { 0.0f, 0.0f };
        m_offscreenRect.size     = t_size;

        const SkyColorSet colors{ t_context.random.from(m_skyColors) };

        m_skyVerts.clear();
        util::appendTriangleVerts(m_offscreenRect, m_skyVerts, colors.bot);
        m_skyVerts.at(0).color = colors.top;
        m_skyVerts.at(1).color = colors.top;
        m_skyVerts.at(4).color = colors.top;

        // load textures
        util::TextureLoader::load(
            m_cloud1Texture,
            (t_context.setting.media_path / "image" / "background" / "cloud-1.png"),
            true);

        util::TextureLoader::load(
            m_cloud2Texture,
            (t_context.setting.media_path / "image" / "background" / "cloud-2.png"),
            true);

        util::TextureLoader::load(
            m_cloud3Texture,
            (t_context.setting.media_path / "image" / "background" / "cloud-3.png"),
            true);

        util::TextureLoader::load(
            m_moonTexture,
            (t_context.setting.media_path / "image" / "background" / "moon.png"),
            true);

        util::TextureLoader::load(
            m_sunTexture,
            (t_context.setting.media_path / "image" / "background" / "sun.png"),
            true);

        // cloud animations
        m_cloudAnims.clear();
        const std::size_t cloudCount{ t_context.random.fromTo(4_st, 8_st) };
        for (std::size_t i{ 0 }; i < cloudCount; ++i)
        {
            CloudAnim & anim{ m_cloudAnims.emplace_back() };
            const int imageSelectNumber{ t_context.random.fromTo(1, 3) };
            if (1 == imageSelectNumber)
            {
                anim.sprite.setTexture(m_cloud1Texture, true);
            }
            else if (2 == imageSelectNumber)
            {
                anim.sprite.setTexture(m_cloud2Texture, true);
            }
            else
            {
                anim.sprite.setTexture(m_cloud3Texture, true);
            }

            const float scale{ t_context.random.fromTo(0.5f, 1.0f) }; // TODO account for resolution
            anim.sprite.scale({ scale, scale });

            util::setOriginToCenter(anim.sprite);
            if (t_context.random.boolean())
            {
                anim.sprite.scale({ -1.0f, 1.0f });
            }

            const sf::Vector2f position{
                t_context.random.fromTo(0.0f, util::right(m_offscreenRect)),
                t_context.random.fromTo(0.0f, (m_offscreenRect.size.y * 0.75f))
            };

            anim.sprite.setPosition(position);

            anim.speed = t_context.random.fromTo(5.0f, 20.0f);
        }

        // moon
        m_moonSprite.setTexture(m_moonTexture, true);
        util::setOriginToCenter(m_moonSprite);
        m_moonSprite.scale({ 0.5f, 0.5f }); // TODO account for resolution

        if (t_context.random.boolean())
        {
            m_moonSprite.scale({ -1.0f, 1.0f });
        }

        const sf::FloatRect moonBounds{ m_moonSprite.getGlobalBounds() };
        sf::FloatRect moonRect;
        moonRect.position = moonBounds.size;
        moonRect.size     = (m_offscreenRect.size - (moonBounds.size * 2.0f));
        moonRect.size.y -= moonBounds.size.y;

        m_moonSprite.setPosition(
            { t_context.random.fromTo(moonRect.position.x, util::right(moonRect)),
              t_context.random.fromTo(moonRect.position.y, util::bottom(moonRect)) });

        // sun
        m_sunSprite.setTexture(m_sunTexture, true);
        util::setOriginToCenter(m_sunSprite);
        m_sunSprite.scale({ 0.75f, 0.75f }); // TODO account for resolution

        if (t_context.random.boolean())
        {
            m_sunSprite.scale({ -1.0f, 1.0f });
        }

        const sf::FloatRect sunBounds{ m_sunSprite.getGlobalBounds() };
        sf::FloatRect sunRect;
        sunRect.position = sunBounds.size;
        sunRect.size     = (m_offscreenRect.size - (sunBounds.size * 2.0f));
        sunRect.size.y -= sunBounds.size.y;

        m_sunSprite.setPosition(
            { t_context.random.fromTo(sunRect.position.x, util::right(sunRect)),
              t_context.random.fromTo(sunRect.position.y, util::bottom(sunRect)) });
    }

    void SkyBackground::update(const Context & t_context, const float t_elapsedSec)
    {
        for (CloudAnim & anim : m_cloudAnims)
        {
            anim.sprite.move({ -(anim.speed * t_elapsedSec), 0.0f });

            if (not m_offscreenRect.findIntersection(anim.sprite.getGlobalBounds()))
            {
                anim.sprite.setPosition(
                    { (util::right(m_offscreenRect) +
                       (anim.sprite.getGlobalBounds().size.x * 0.5f)),
                      t_context.random.fromTo(0.0f, (m_offscreenRect.size.y * 0.75f)) });
            }
        }
    }

    void SkyBackground::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        if (not m_skyVerts.empty())
        {
            t_target.draw(
                &m_skyVerts[0], m_skyVerts.size(), sf::PrimitiveType::Triangles, t_states);
        }

        if (m_willShowMoon)
        {
            t_target.draw(m_moonSprite, t_states);
        }

        if (m_willShowSun)
        {
            t_target.draw(m_sunSprite, t_states);
        }

        for (const CloudAnim & anim : m_cloudAnims)
        {
            t_target.draw(anim.sprite, t_states);
        }
    }

} // namespace shadowman
