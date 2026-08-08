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
        : m_skyVerts{}
        , m_cloud1Texture{}
        , m_cloud2Texture{}
        , m_cloud3Texture{}
        , m_moonTexture{}
        , m_moonSprite{ m_moonTexture }
        , m_cloudAnims{}
        , m_willShowMoon{ false }
    {}

    void SkyBackground::setup(const Context & t_context)
    {
        // sky color verts
        const sf::Color botColor(136, 136, 136);
        const sf::Color topColor(254, 204, 0);

        util::appendTriangleVerts(t_context.layout.wholeRect(), m_skyVerts, botColor);

        m_skyVerts.at(0).color = topColor;
        m_skyVerts.at(1).color = topColor;
        m_skyVerts.at(4).color = topColor;

        // load textures
        util::TextureLoader::load(
            m_cloud1Texture, (t_context.setting.media_path / "image" / "cloud-1.png"), true);

        util::TextureLoader::load(
            m_cloud2Texture, (t_context.setting.media_path / "image" / "cloud-2.png"), true);

        util::TextureLoader::load(
            m_cloud3Texture, (t_context.setting.media_path / "image" / "cloud-3.png"), true);

        util::TextureLoader::load(
            m_moonTexture, (t_context.setting.media_path / "image" / "moon.png"), true);

        // cloud animations
        const sf::FloatRect wholeRect{ t_context.layout.wholeRect() };
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

            const float scale{ t_context.random.fromTo(0.5f, 1.0f) };
            anim.sprite.scale({ scale, scale });

            util::setOriginToCenter(anim.sprite);
            if (t_context.random.boolean())
            {
                anim.sprite.scale({ -1.0f, 1.0f });
            }

            const sf::Vector2f position{ t_context.random.fromTo(0.0f, util::right(wholeRect)),
                                         t_context.random.fromTo(
                                             0.0f, (wholeRect.size.y * 0.75f)) };

            anim.sprite.setPosition(position);

            anim.speed = t_context.random.fromTo(5.0f, 20.0f);
        }

        // moon
        m_willShowMoon = true;
        // t_context.random.boolean();
        if (m_willShowMoon)
        {
            m_moonSprite.setTexture(m_moonTexture, true);
            util::setOriginToCenter(m_moonSprite);
            m_moonSprite.scale({ 0.5f, 0.5f });

            if (t_context.random.boolean())
            {
                m_moonSprite.scale({ -1.0f, 1.0f });
            }

            const sf::FloatRect moonBounds{ m_moonSprite.getGlobalBounds() };
            sf::FloatRect rect;
            rect.position = moonBounds.size;
            rect.size     = (wholeRect.size - (moonBounds.size * 2.0f));
            rect.size.y -= moonBounds.size.y;

            m_moonSprite.setPosition(
                { t_context.random.fromTo(rect.position.x, util::right(rect)),
                  t_context.random.fromTo(rect.position.y, util::bottom(rect)) });
        }
    }

    void SkyBackground::update(const Context & t_context, const float t_elapsedSec)
    {
        const sf::FloatRect wholeRect{ t_context.layout.wholeRect() };

        for (CloudAnim & anim : m_cloudAnims)
        {
            anim.sprite.move({ -(anim.speed * t_elapsedSec), 0.0f });

            if (not wholeRect.findIntersection(anim.sprite.getGlobalBounds()))
            {
                anim.sprite.setPosition(
                    { (util::right(wholeRect) + (anim.sprite.getGlobalBounds().size.x * 0.5f)),
                      t_context.random.fromTo(0.0f, (wholeRect.size.y * 0.75f)) });
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

        for (const CloudAnim & anim : m_cloudAnims)
        {
            t_target.draw(anim.sprite, t_states);
        }
    }

} // namespace shadowman
