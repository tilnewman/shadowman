#ifndef AVATAR_PLAYER_INFO_HPP_INCLUDED
#define AVATAR_PLAYER_INFO_HPP_INCLUDED
//
// player-info.hpp
//
#include <algorithm>

namespace shadowman
{

    class PlayerInfo
    {
      public:
        PlayerInfo()
            : m_healthMax{ 0 }
            , m_health{ 0 }
        {
            reset();
        }

        [[nodiscard]] constexpr int health() const noexcept { return m_health; }
        [[nodiscard]] constexpr int healthMax() const noexcept { return m_healthMax; }

        constexpr void healthAdjust(const int t_adj)
        {
            m_health = std::clamp((m_health + t_adj), 0, m_healthMax);
        }

        constexpr void reset()
        {
            m_healthMax = 5;
            m_health    = 5;
        }

      private:
        int m_healthMax;
        int m_health;
    };

} // namespace shadowman

#endif // AVATAR_PLAYER_INFO_HPP_INCLUDED
