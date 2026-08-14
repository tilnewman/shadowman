//
// screen-shaker.cpp
//
#include "screen-shaker.hpp"

#include "subsystem/context.hpp"

namespace shadowman
{

    ScreenShaker::ScreenShaker()
        : m_isEnabled{ false }
        , m_horizSlider{}
        , m_vertSlider{}
    {}

    void ScreenShaker::setup(const Context & t_context)
    {
        const float range{ 10.0f };
        const float min{ range * -0.5f };
        const float max{ range * 0.5f };

        m_horizSlider = util::SliderOscillator<float>(
            min, max, t_context.random.fromTo(10.0f, 20.0f), t_context.random.fromTo(min, max));

        m_vertSlider = util::SliderOscillator<float>(
            min, max, t_context.random.fromTo(10.0f, 20.0f), t_context.random.fromTo(min, max));
    }

    void ScreenShaker::update(const float t_elapsedSec)
    {
        m_horizSlider.update(t_elapsedSec);
        m_vertSlider.update(t_elapsedSec);
    }

    const sf::Vector2f ScreenShaker::offset() const
    {
        if (m_isEnabled)
        {
            return sf::Vector2f{ m_horizSlider.value(), m_vertSlider.value() };
        }
        else
        {
            return sf::Vector2f{ 0.0f, 0.0f };
        }
    }

} // namespace shadowman
