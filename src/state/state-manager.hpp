#ifndef STATE_MANAGER_HPP_INCLUDED
#define STATE_MANAGER_HPP_INCLUDED
//
// state-manager.hpp
//
#include "state/state.hpp"

#include <memory>
#include <optional>

namespace shadowman
{

    struct Context;

    class StateManager
    {
      public:
        StateManager();

        [[nodiscard]] inline IState & current() const { return *m_currentStateUPtr; }

        constexpr void setChangePending(const State t_newState) noexcept
        {
            m_changePendingOpt = t_newState;
        }

        [[nodiscard]] constexpr bool isChangePending() const noexcept
        {
            return m_changePendingOpt.has_value();
        }

        // only coordinator.cpp should call this after all updates are finished()
        void changeIfPending(const Context & context);

      private:
        [[nodiscard]] static std::unique_ptr<IState> factory(const State t_state);

        std::unique_ptr<IState> m_currentStateUPtr;
        std::optional<State> m_changePendingOpt;
    };

} // namespace bramblefore

#endif // STATE_MANAGER_HPP_INCLUDED
