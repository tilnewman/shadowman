//
// state-play.cpp
//
#include "state-play.hpp"

#include "avatar/avatar.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace shadowman
{

    StatePlay::StatePlay()
        : m_skyBackground{}
    {}

    void StatePlay::onEnter(const Context & t_context) { m_skyBackground.setup(t_context); }

    void StatePlay::onExit(const Context &) {}

    void StatePlay::update(const Context & t_context, const float t_elapsedSec)
    {
        t_context.avatar.update(t_context, t_elapsedSec);
        m_skyBackground.update(t_context, t_elapsedSec);
    }

    void StatePlay::handleEvent(const Context &, const sf::Event &) {}

    void StatePlay::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        m_skyBackground.draw(t_target, t_states);
        t_context.avatar.draw(t_target, t_states);
    }

} // namespace shadowman