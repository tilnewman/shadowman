//
// screen-layout.cpp
//
#include "subsystem/screen-layout.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "util/sfml-util.hpp"

namespace shadowman
{

    ScreenLayout::ScreenLayout()
        : m_wholeRect{}
    {}

    void ScreenLayout::setup(const sf::Vector2u & t_windowSize)
    {
        m_wholeRect = { { 0.0f, 0.0f },
                        { static_cast<float>(t_windowSize.x),
                          static_cast<float>(t_windowSize.y) } };

        m_mapRect = util::scaleRectInPlaceCopy(m_wholeRect, 0.75f);
    }

    float ScreenLayout::scaleBasedOnResolution(
        const Context & t_context, const float t_scale, const CalcOrder t_order) const
    {
        // This is the resolution on my macbook originally used when starting the game.
        // The physics values in settings.hpp work as is when this is the resolution.
        const float original{ std::sqrt(2056.0f * 1329.0f) };

        const float current{ std::sqrt(
            static_cast<float>(
                t_context.setting.video_mode.size.x * t_context.setting.video_mode.size.y)) };

        if (CalcOrder::Normal == t_order)
        {
            return ((t_scale * current) / original);
        }
        else
        {
            return ((t_scale * original) / current);
        }
    }

} // namespace shadowman
