//
// state-intro.cpp
//
#include "state-intro.hpp"

#include "shadowman/settings.hpp"
#include "state/state-manager.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
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
    {}

    void StateIntro::onEnter(const Context & t_context)
    {
        t_context.audio.play("duuuh");

        m_skyBackground.setup(t_context);

        m_fadeRectangle.setFillColor(sf::Color::Black);
        m_fadeRectangle.setOutlineThickness(0.0f);
        m_fadeRectangle.setPosition({ 0.0f, 0.0f });
        m_fadeRectangle.setSize(t_context.layout.wholeRect().size);
    }

    void StateIntro::onExit(const Context & t_context) { t_context.audio.stop("duuuh"); }

    void StateIntro::update(const Context &, const float t_elapsedSec)
    {
        m_elapsedSec += t_elapsedSec;
        if (m_elapsedSec > 1.0f)
        {
            std::uint8_t alpha{ m_fadeRectangle.getFillColor().a };
            if (alpha > 0)
            {
                --alpha;
            }
            m_fadeRectangle.setFillColor(sf::Color(0, 0, 0, alpha));
        }
    }

    void StateIntro::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        if (const auto * keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            t_context.state.setChangePending(State::Play);
        }
    }

    void StateIntro::draw(
        const Context &, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        m_skyBackground.draw(t_target, t_states);
        t_target.draw(m_fadeRectangle, t_states);
    }

} // namespace shadowman
