//
// state-play.cpp
//
#include "state-play.hpp"

#include "avatar/avatar.hpp"
#include "enemy/fly-manager.hpp"
#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/smoke.hpp"
#include "util/music-player.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Event.hpp>

namespace shadowman
{

    StatePlay::StatePlay() {}

    void StatePlay::onEnter(const Context & t_context)
    {
        t_context.level.load(t_context, "level-1.json");
        t_context.music.start("music.ogg");
    }

    void StatePlay::onExit(const Context & t_context) { t_context.music.stop("music.ogg"); }

    void StatePlay::update(const Context & t_context, const float t_elapsedSec)
    {
        t_context.avatar.update(t_context, t_elapsedSec);
        t_context.fly.update(t_context, t_elapsedSec);
        t_context.level.update(t_context, t_elapsedSec);
        t_context.smoke.update(t_context, t_elapsedSec);
    }

    void StatePlay::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        t_context.level.handleEvent(t_context, t_event);
    }

    void StatePlay::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_context.level.draw(t_context, t_target, t_states);
    }

} // namespace shadowman