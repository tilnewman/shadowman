//
// state-manager.hpp
//
#include "state-manager.hpp"

#include "state/state-credits.hpp"
#include "state/state-play.hpp"
#include "state/state-intro.hpp"

#include <stdexcept>

namespace shadowman
{

    StateManager::StateManager()
        : m_currentStateUPtr{ factory(State::Startup) }
        , m_changePendingOpt{ std::nullopt }
    {}

    void StateManager::changeIfPending(const Context & t_context)
    {
        if (!m_changePendingOpt)
        {
            return;
        }

        m_currentStateUPtr->onExit(t_context);

        m_currentStateUPtr = factory(m_changePendingOpt.value());
        m_changePendingOpt = std::nullopt;

        m_currentStateUPtr->onEnter(t_context);
    }

    std::unique_ptr<IState> StateManager::factory(const State state)
    {
        // clang-format off
        switch (state)
        {
            case State::Startup:      { return std::make_unique<StateStartup>();  }
            case State::Introduction: { return std::make_unique<StateIntro>();    }
            case State::Play:         { return std::make_unique<StatePlay>();     }
            case State::Credits:      { return std::make_unique<StateCredits>();  }
            case State::Shutdown:     { return std::make_unique<StateShutdown>(); }
            default:                    
            { 
                throw std::runtime_error("StateManager::factory() given an unknown State.");
            }

        }
        // clang-format on
    }

} // namespace shadowman
