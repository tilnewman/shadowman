#ifndef STATE_STATE_PLAY_HPP_INCLUDED
#define STATE_STATE_PLAY_HPP_INCLUDED
//
// state-play.hpp
//
#include "state/state.hpp"
#include "subsystem/sky-background.hpp"

#include <SFML/Graphics/RenderStates.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{

    struct Context;

    class StatePlay final : public IState
    {
      public:
        StatePlay();
        ~StatePlay() final = default;

        State which() const final { return State::Play; }

        void onEnter(const Context & t_context) final;
        void onExit(const Context & t_context) final;
        void update(const Context & t_context, const float t_elapsedSec) final;
        void handleEvent(const Context & t_context, const sf::Event & t_event) final;

        void draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
            const final;

      private:
        SkyBackground m_skyBackground;
    };

} // namespace shadowman

#endif // STATE_STATE_PLAY_HPP_INCLUDED
