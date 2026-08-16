//
// state-intro.cpp
//
#include "state-intro.hpp"

#include "shadowman/settings.hpp"
#include "state/state-manager.hpp"
#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/sfml-util.hpp"
#include "util/sound-player.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    StateIntro::StateIntro()
        : m_paperTexture{}
        , m_paperSprite{ m_paperTexture }
        , m_fadeRectangle{}
        , m_elapsedSec{ 0.0f }
        , m_skyBackground{}
        , m_textPack{}
        , m_willFadeOut{ false }
    {}

    void StateIntro::onEnter(const Context & t_context)
    {
        t_context.audio.play("duuuh");

        m_skyBackground.setup(t_context);

        m_fadeRectangle.setFillColor(sf::Color::Black);
        m_fadeRectangle.setOutlineThickness(0.0f);
        m_fadeRectangle.setPosition({ 0.0f, 0.0f });
        m_fadeRectangle.setSize(t_context.layout.wholeRect().size);

        util::TextureLoader::load(
            m_paperTexture,
            (t_context.setting.media_path / "image" / "paper" / "paper-vert.png"),
            true);

        m_paperSprite.setTexture(m_paperTexture, true);
        m_paperSprite.setColor(sf::Color(255, 255, 255, 0));

        const float scale{ t_context.layout.scaleBasedOnResolution(t_context, 1.5f) };
        m_paperSprite.setScale({ scale, scale });

        util::centerInside(m_paperSprite, t_context.layout.wholeRect());

        const TextDetails textDetails(Font::Dashley, FontSize::Large, sf::Color::Black);

        const std::string textStr{
            "Something is...wrong. It feels empty. Plants are wild. Pillars of stone float. Colors "
            "in the sky are tinted green. Monsters roam. <p> <p> It's time for someone to set "
            "things right. <p> <p> Someone with a knife."
        };

        sf::FloatRect textRect({ 0.0f, 0.0f }, { 369.0f, 416.0f });
        util::scaleRectInPlace(textRect, scale);
        textRect.position = m_paperSprite.getPosition();
        textRect.position += sf::Vector2f{ 32.0f, 32.0f };

        m_textPack = TextLayout::typeset(t_context, textStr, textRect, textDetails);

        for (sf::Text & text : m_textPack.texts)
        {
            text.setFillColor(sf::Color::Transparent);
        }
    }

    void StateIntro::onExit(const Context & t_context) { t_context.audio.stop("duuuh"); }

    void StateIntro::update(const Context & t_context, const float t_elapsedSec)
    {
        m_skyBackground.update(t_context, t_elapsedSec);

        m_elapsedSec += t_elapsedSec;

        if (m_willFadeOut)
        {
            std::uint8_t fadeOutAlpha{ m_fadeRectangle.getFillColor().a };
            if (fadeOutAlpha < 254)
            {
                fadeOutAlpha += 2;
                m_fadeRectangle.setFillColor(sf::Color(0, 0, 0, fadeOutAlpha));
            }
            else
            {
                t_context.state.setChangePending(State::Play);
            }
        }
        else if (m_elapsedSec > 1.0f)
        {
            std::uint8_t rectAlpha{ m_fadeRectangle.getFillColor().a };
            if (rectAlpha > 0)
            {
                --rectAlpha;
                m_fadeRectangle.setFillColor(sf::Color(0, 0, 0, rectAlpha));
            }
            else
            {
                std::uint8_t spriteAlpha{ m_paperSprite.getColor().a };
                if (spriteAlpha < 255)
                {
                    ++spriteAlpha;

                    m_paperSprite.setColor(sf::Color(255, 255, 255, spriteAlpha));

                    for (sf::Text & text : m_textPack.texts)
                    {
                        text.setFillColor(sf::Color(0, 0, 0, spriteAlpha));
                    }
                }
                else
                {
                    if (m_elapsedSec > 20.0f)
                    {
                        m_willFadeOut = true;
                    }
                }
            }
        }
    }

    void StateIntro::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (t_event.is<sf::Event::KeyPressed>())
        {
            t_context.state.setChangePending(State::Play);
        }
    }

    void StateIntro::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        m_skyBackground.draw(t_target, t_states);

        t_target.draw(m_paperSprite, t_states);

        for (const sf::Text & text : m_textPack.texts)
        {
            t_target.draw(text, t_states);
        }

        t_target.draw(m_fadeRectangle, t_states);
    }

} // namespace shadowman
