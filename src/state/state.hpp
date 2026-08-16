#ifndef STATE_STATE_HPP_INCLUDED
#define STATE_STATE_HPP_INCLUDED
//
// state.hpp
//
#include <SFML/Graphics/RenderStates.hpp>

namespace sf
{
    class Event;
    class RenderTarget;
} // namespace sf

namespace shadowman
{
    struct Context;

    enum class State : unsigned char
    {
        Startup,
        Introduction,
        Play,
        Credits,
        Shutdown
    };

    struct IState
    {
        virtual ~IState() = default;

        virtual State which() const                                                    = 0;
        virtual void onEnter(const Context & t_context)                                = 0;
        virtual void onExit(const Context & t_context)                                 = 0;
        virtual void update(const Context & t_context, const float t_elapsedSec)       = 0;
        virtual void handleEvent(const Context & t_context, const sf::Event & t_event) = 0;

        virtual void draw(
            const Context & t_context,
            sf::RenderTarget & t_target,
            sf::RenderStates t_states) const = 0;
    };

    class StateStartup final : public IState
    {
      public:
        ~StateStartup() final = default;

        State which() const final { return State::Startup; }
        void onEnter(const Context &) final {}
        void onExit(const Context &) final {}
        void update(const Context &, const float) final {}
        void handleEvent(const Context &, const sf::Event &) final {}
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates) const final {}
    };

    class StateShutdown final : public IState
    {
      public:
        ~StateShutdown() final = default;

        State which() const final { return State::Shutdown; }
        void onEnter(const Context &) final {}
        void onExit(const Context &) final {}
        void update(const Context &, const float) final {}
        void handleEvent(const Context &, const sf::Event &) final {}
        void draw(const Context &, sf::RenderTarget &, sf::RenderStates) const final {}
    };

} // namespace shadowman

#endif // STATE_STATE_HPP_INCLUDED
