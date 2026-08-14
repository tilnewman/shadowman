#ifndef SUBSYSTEM_SCREEN_SHAKER_HPP_INCLUDED
#define SUBSYSTEM_SCREEN_SHAKER_HPP_INCLUDED
//
// screen-shaker.hpp
//
#include "util/sliders.hpp"

#include <SFML/System/Vector2.hpp>

namespace shadowman
{
    struct Context;

    class ScreenShaker
    {
      public:
        ScreenShaker();

        constexpr void start() noexcept { m_isEnabled = true; }
        constexpr void stop() noexcept { m_isEnabled = false; }

        void setup(const Context & t_context);
        void update(const float t_elapsedSec);

        [[nodiscard]] const sf::Vector2f offset() const;

      private:
        bool m_isEnabled;
        util::SliderOscillator<float> m_horizSlider;
        util::SliderOscillator<float> m_vertSlider;
    };

} // namespace shadowman

#endif // SUBSYSTEM_SCREEN_SHAKER_HPP_INCLUDED
