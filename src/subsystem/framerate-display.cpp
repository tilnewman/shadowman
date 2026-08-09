//
// framerate-display.hpp
//
#include "framerate-display.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/sfml-defaults.hpp"
#include "util/stats-display.hpp"

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    FramerateDisplay::FramerateDisplay()
        : m_text{ util::SfmlDefaults::instance().font() }
        , m_elapsedSec{ 0.0f }
        , m_fpsValues{}
    {
        m_fpsValues.reserve(120);
    }

    void FramerateDisplay::update(const Context & t_context, const float t_elapsedSec)
    {
        m_fpsValues.push_back(static_cast<std::size_t>(1.0f / t_elapsedSec));

        m_elapsedSec += (1.0f / t_context.setting.framerate);
        if (m_elapsedSec > 1.0f)
        {
            const auto stats{ util::makeStats(m_fpsValues) };
            m_text = t_context.font.makeText(Font::General, FontSize::Medium, stats.toString());

            m_text.setPosition(
                { 0.0f, (t_context.layout.wholeRect().size.y - m_text.getGlobalBounds().size.y) });

            m_elapsedSec = 0.0f;
            m_fpsValues.clear();
        }
    }

    void FramerateDisplay::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_text, t_states);
    }

} // namespace shadowman
