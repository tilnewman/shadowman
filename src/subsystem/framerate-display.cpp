//
// framerate-display.hpp
//
#include "framerate-display.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/sfml-defaults.hpp"

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    FramerateDisplay::FramerateDisplay()
        : m_text{ util::SfmlDefaults::instance().font() }
        , m_frameCount{ 0 }
        , m_elapsedSec{ 0.0f }
    {}

    void FramerateDisplay::update(const Context & t_context, const float t_elapsedSec)
    {
        ++m_frameCount;

        m_elapsedSec += t_elapsedSec;
        if (m_elapsedSec > 1.0f)
        {
            const std::size_t framerate{ static_cast<std::size_t>(
                static_cast<float>(m_frameCount) / m_elapsedSec) };

            m_text =
                t_context.font.makeText(Font::General, FontSize::Medium, std::to_string(framerate));

            m_text.setPosition(
                { 0.0f, (t_context.layout.wholeRect().size.y - m_text.getGlobalBounds().size.y) });

            m_elapsedSec = 0.0f;
            m_frameCount = 0;
        }
    }

    void FramerateDisplay::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_text, t_states);
    }

} // namespace shadowman
