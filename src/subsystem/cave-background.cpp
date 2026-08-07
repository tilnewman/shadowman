//
// cave-background.cpp
//
#include "cave-background.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/random.hpp"
#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

namespace shadowman
{
    CaveBackground::CaveBackground()
        : m_caveBgTexture{}
        , m_stalactiteTexture{}
        , m_sprite1{ util::SfmlDefaults::instance().texture() }
        , m_sprite2{ util::SfmlDefaults::instance().texture() }
        , m_offscreenTexture1{}
        , m_offscreenTexture2{}
        , m_backgroundColor{ 8, 37, 47 }
        , m_pillarColor{ 12, 58, 73 }
    {}

    void CaveBackground::setup(const Context & t_context)
    {
        util::TextureLoader::load(
            m_caveBgTexture,
            (t_context.setting.media_path / "image" / "cave-background.png"),
            true);

        util::TextureLoader::load(
            m_stalactiteTexture,
            (t_context.setting.media_path / "image" / "stalactites.png"),
            true);

        composeBackground(t_context, m_offscreenTexture1, m_sprite1);
    }

    void CaveBackground::move(const float t_amount)
    {
        m_sprite1.move({ -t_amount, 0.0f });
        m_sprite2.move({ -t_amount, 0.0f });
    }

    void CaveBackground::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite1, t_states);
        // t_target.draw(m_sprite2, t_states);
    }

    void CaveBackground::composeBackground(
        const Context & t_context, sf::RenderTexture & t_offscreenTexture, sf::Sprite & t_sprite)
    {
        if (not t_offscreenTexture.resize({ 2048u, 1024u }))
        {
            M_LOG("Your graphics card sucks.  Failed to make an offscreen sf::RenderTexture.");
            return;
        }

        t_offscreenTexture.setSmooth(true);
        t_offscreenTexture.clear(m_backgroundColor);
        composePillars(t_context, t_offscreenTexture);
        composeStalactites(t_context, t_offscreenTexture);
        t_offscreenTexture.display();
        t_sprite.setTexture(t_offscreenTexture.getTexture(), true);
        t_sprite.setPosition({ 0.0f, 0.0f });
        const float scale{ t_context.layout.wholeRect().size.y / t_sprite.getLocalBounds().size.y };
        t_sprite.scale({ scale, scale });
    }

    void CaveBackground::composePillars(
        const Context & t_context, sf::RenderTexture & t_offscreenTexture)
    {
        int horizPos{ 0 };
        const sf::IntRect leftRect{ getRandomPillarRectLeft(t_context) };
        sf::Sprite transferSprite(m_caveBgTexture);
        transferSprite.setTextureRect(leftRect);
        t_offscreenTexture.draw(transferSprite);
        horizPos += leftRect.size.x;

        const int betweenDistance{ t_context.random.fromTo(4, 300) };
        const sf::FloatRect betweenRect{ sf::IntRect({ horizPos, 0 }, { betweenDistance, 1024 }) };
        util::drawRectangleShape(t_offscreenTexture, betweenRect, true, m_pillarColor);
        horizPos += betweenDistance;

        const sf::IntRect rightRect{ getRandomPillarRectRight(t_context) };
        transferSprite.setTextureRect(rightRect);
        transferSprite.setPosition({ static_cast<float>(horizPos), 0.0f });
        t_offscreenTexture.draw(transferSprite);
        horizPos += rightRect.size.x;

        // const int afterDistance{ t_context.random.fromTo(4, 512) };
    }

    void CaveBackground::composeStalactites(
        const Context & t_context, sf::RenderTexture & t_offscreenTexture)
    {}

    const sf::IntRect CaveBackground::getRandomPillarRectLeft(const Context & t_context) const
    {
        const int randomSelection{ t_context.random.fromTo(1, 3) };
        if (1 == randomSelection)
        {
            return { { 0, 0 }, { 704, 1024 } };
        }
        else if (2 == randomSelection)
        {
            return { { 0, 1024 }, { 448, 1024 } };
        }
        else
        {
            return { { 1088, 1024 }, { 335, 1024 } };
        }
    }

    const sf::IntRect CaveBackground::getRandomPillarRectRight(const Context & t_context) const
    {
        const int randomSelection{ t_context.random.fromTo(1, 3) };
        if (1 == randomSelection)
        {
            return { { 832, 0 }, { 320, 1024 } };
        }
        else if (2 == randomSelection)
        {
            return { { 512, 1024 }, { 512, 1024 } };
        }
        else
        {
            return { { 1472, 1024 }, { 358, 1024 } };
        }
    }

} // namespace shadowman
