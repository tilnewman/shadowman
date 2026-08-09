#ifndef UTIL_STATS_DISPLAY_HPP_INCLUDED
#define UTIL_STATS_DISPLAY_HPP_INCLUDED
//
// stats-display.hpp
//
#include "util/check-macros.hpp"
#include "util/sfml-util.hpp"

#include <iostream>
#include <string>

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace util
{

    template <typename data_t>
    class GraphDisplay : public sf::Drawable
    {
      public:
        explicit GraphDisplay(
            const std::vector<data_t> & t_data,
            const sf::Vector2u & t_size = { 1000, 500 },
            const std::uint8_t t_alpha  = 255)
            : m_size{ t_size }
            , m_data{ t_data }
            , m_alpha{ t_alpha }
            , m_renderTexture()
            , m_backgroundColor{ 22, 25, 28 }
            , m_dataBarColor{ 38, 120, 254 }
            , m_dataBarColorError{ 255, 32, 32 }
            , m_averageLineColor{ 255, 255, 255, 64 }
            , m_wasDatasetChanged{ false }
        {
            // reduce data set to fit in the size given
            while (m_data.size() > static_cast<std::size_t>(m_size.x))
            {
                m_data = makeHalfSizeDataSet(m_data);
            }

            m_wasDatasetChanged = (t_data.size() != m_data.size());

            // find min/max/avg/std_dev
            const util::Stats<data_t> stats = util::makeStats(m_data);

            // convert data values into data bar rects
            std::vector<sf::FloatRect> dataBarRects;
            populateDataBarRects(stats.max, dataBarRects);

            // create render texture of the correct size to fit the data -not the m_size.x
            const unsigned dataWidth = static_cast<unsigned>(util::right(dataBarRects.back()));
            if (!m_renderTexture.resize({ dataWidth, m_size.y }))
            {
                std::cout << "GraphDisplay's sf::RenderTexture::create failed for size="
                          << dataWidth << "x" << m_size.y
                          << ". So the graph will show solid magenta.  The color of pure evil.\n";

                M_CHECK(m_renderTexture.resize({ 4u, 4u }), "can't create 4x4 RenderTexture");
                m_renderTexture.clear(sf::Color::Magenta);
                m_renderTexture.display();
            }
            else
            {
                // draw graph
                m_renderTexture.clear(m_backgroundColor);

                const sf::Color & barColorToUse =
                    ((m_wasDatasetChanged) ? m_dataBarColorError : m_dataBarColor);

                drawGraph(m_renderTexture, barColorToUse, m_backgroundColor, dataBarRects);
                drawAverageLine(dataWidth, stats);
                m_renderTexture.display();
            }
        }

        void draw(sf::RenderTarget & t_renderTarget, sf::RenderStates t_states) const override
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::scale(sprite, sf::Vector2f{ m_size });

            const sf::Color color{ 255, 255, 255, m_alpha };
            sprite.setColor(color);

            t_renderTarget.draw(sprite, t_states);
        }

        void draw(
            const sf::Vector2f & t_position,
            sf::RenderTarget & t_renderTarget,
            const sf::RenderStates t_states = {}) const
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::scale(sprite, sf::Vector2f{ m_size });
            sprite.setPosition(t_position);

            const sf::Color color{ 255, 255, 255, m_alpha };
            sprite.setColor(color);

            t_renderTarget.draw(sprite, t_states);
        }

        void saveToFile(const std::string & t_filename) const
        {
            if (!m_renderTexture.getTexture().copyToImage().saveToFile(t_filename))
            {
                std::cout << "Error:  StatDisplay's sf::Image::saveToFile(\"" << t_filename
                          << "\") failed.\n";
            }
        }

        static void makeAndSavePNG(
            const std::string & t_filename,
            const std::vector<data_t> & t_data,
            const sf::Vector2u & t_size = { 1000, 500 })
        {
            const GraphDisplay<data_t> graphDisplay(t_data, t_size);
            graphDisplay.saveToFile(t_filename);
        }

        bool wasDatasetChanged() const noexcept { return m_wasDatasetChanged; }

        [[nodiscard]] const sf::Texture & texture() const noexcept
        {
            return m_renderTexture.getTexture();
        }

        [[nodiscard]] const sf::Vector2u size() const noexcept { return m_size; }
        [[nodiscard]] const std::vector<data_t> & data() const noexcept { return m_data; }

      private:
        [[nodiscard]] std::vector<data_t>
            makeHalfSizeDataSet(const std::vector<data_t> & t_oldValues) const
        {
            std::vector<data_t> newValues;
            newValues.reserve((t_oldValues.size() / 2) + 1);

            std::size_t oldIndex = 0;
            while (oldIndex < t_oldValues.size())
            {
                const data_t a = t_oldValues.at(oldIndex++);

                if (oldIndex >= t_oldValues.size())
                {
                    newValues.push_back(a);
                    break;
                }

                const data_t b       = t_oldValues.at(oldIndex++);
                const data_t average = (a + b) / data_t(2);
                newValues.push_back(average);
            }

            return newValues;
        }

        [[nodiscard]] float calcDataBarWidth() const noexcept
        {
            if (m_data.size() == 0)
            {
                return 1.0f;
            }

            const float width =
                std::floor(static_cast<float>(m_size.x) / static_cast<float>(m_data.size()));

            if (width < 1.0f)
            {
                return 1.0f;
            }
            else
            {
                return width;
            }
        }

        void populateDataBarRects(
            const data_t t_maxValue, std::vector<sf::FloatRect> & t_dataBarRects) const
        {
            if (t_maxValue <= data_t(0))
            {
                return;
            }

            t_dataBarRects.clear();
            t_dataBarRects.reserve(m_data.size());

            const float dataBarWidth = calcDataBarWidth();

            float posLeft = 0.0f;
            for (const data_t value : m_data)
            {
                const float valueRatio =
                    (static_cast<float>(value) / static_cast<float>(t_maxValue));

                sf::FloatRect dataBarRect;
                dataBarRect.position.x = posLeft;
                dataBarRect.size.x     = dataBarWidth;
                dataBarRect.size.y     = (static_cast<float>(m_size.y) * valueRatio);
                dataBarRect.position.y = (static_cast<float>(m_size.y) - dataBarRect.size.y);

                t_dataBarRects.push_back(dataBarRect);

                posLeft += dataBarRect.size.x;
            }
        }

        void drawGraph(
            sf::RenderTexture & t_renderTexture,
            const sf::Color & t_barColor,
            const sf::Color & t_backgroundColor,
            const std::vector<sf::FloatRect> & t_dataBarRects) const
        {
            sf::RectangleShape rectangle;

            for (const sf::FloatRect & dataBarRect : t_dataBarRects)
            {
                rectangle.setSize(dataBarRect.size);
                rectangle.setPosition(dataBarRect.position);

                rectangle.setFillColor(t_barColor);
                rectangle.setOutlineColor(sf::Color::Transparent);
                rectangle.setOutlineThickness(0.0f);

                t_renderTexture.draw(rectangle);

                // if bars are wide enough then draw dark outline around them because it's pretty
                if (dataBarRect.size.x > 4.0f)
                {
                    rectangle.setOutlineThickness(1.0f);
                    rectangle.setOutlineColor(t_backgroundColor);
                    rectangle.setFillColor(sf::Color::Transparent);

                    t_renderTexture.draw(rectangle);
                }
            }
        }

        void drawAverageLine(const unsigned t_dataWidth, const util::Stats<data_t> & t_stats)
        {
            if ((t_stats.count == 0) || !(t_stats.max > data_t(0)))
            {
                return;
            }

            const float avgLineMagnitude = std::floor(
                (static_cast<float>(t_stats.avg) * static_cast<float>(m_size.y)) /
                static_cast<float>(t_stats.max));

            const float avgLineHeight = (static_cast<float>(m_size.y) - avgLineMagnitude);

            sf::RectangleShape rectangle;
            rectangle.setSize({ static_cast<float>(t_dataWidth), 1.0f });
            rectangle.setPosition({ 0.0f, avgLineHeight });
            rectangle.setFillColor(m_averageLineColor);
            rectangle.setOutlineColor(sf::Color::Transparent);
            rectangle.setOutlineThickness(0.0f);
            m_renderTexture.draw(rectangle);
        }

      private:
        sf::Vector2u m_size;
        std::vector<data_t> m_data;
        std::uint8_t m_alpha;
        sf::RenderTexture m_renderTexture;
        sf::Color m_backgroundColor;
        sf::Color m_dataBarColor;
        sf::Color m_dataBarColorError;
        sf::Color m_averageLineColor;
        bool m_wasDatasetChanged;
    };

    //
    //
    //

    template <typename data_t>
    class StatsDisplay : public sf::Drawable
    {
      public:
        StatsDisplay(
            const std::string & t_title,
            const sf::Font & t_font,
            const std::vector<data_t> & t_data,
            const sf::Vector2u & t_graphSize = { 1000, 500 })
            : m_title{ t_title }
            , m_size{}
            , m_data{ t_data }
            , m_renderTexture{}
            , m_graphDisplay{ t_data, t_graphSize }
            , m_backgroundColor{ 27, 31, 35 }
            , m_borderColor{ 94, 104, 114 }
            , m_titleTextColor{ 245, 246, 249 }
            , m_subTitleTextColor{ 132, 139, 148 }
        {
            const sf::Vector2f graphPadRatio({ 0.1f, 0.25f });
            const sf::Vector2f graphSizeF{ t_graphSize };
            m_size = (graphSizeF + (graphSizeF * graphPadRatio));
            const sf::Vector2u fullSizeU{ m_size };
            const sf::FloatRect fullRect({ 0.0f, 0.0f }, m_size);
            const sf::FloatRect graphRect((graphSizeF * graphPadRatio * 0.5f), graphSizeF);

            const sf::FloatRect borderRect(
                { 1.0f, 1.0f }, { (m_size.x - 2.0f), (m_size.y - 2.0f) });

            sf::FloatRect titleRect(
                { 2.0f, 2.0f }, { (m_size.x - 4.0f), (graphRect.position.y - 3.0f) });

            util::scaleRectInPlace(titleRect, 0.6f);
            sf::Text titleText(t_font);
            titleText.setCharacterSize(50);
            titleText.setFillColor(m_titleTextColor);
            titleText.setString(t_title);
            util::setOriginToPosition(titleText);
            util::fitAndCenterInside(titleText, titleRect);

            const auto stats = util::makeStats(m_data);

            sf::FloatRect subTitleRect({ 2.0f, (util::bottom(graphRect) + 1.0f) }, titleRect.size);

            util::scaleRectInPlace(subTitleRect, 0.5f);
            sf::Text subTitleText(t_font);
            subTitleText.setCharacterSize(50);
            subTitleText.setFillColor(m_subTitleTextColor);
            subTitleText.setString(stats.toString());
            util::setOriginToPosition(subTitleText);
            util::fitAndCenterInside(subTitleText, subTitleRect);

            if (!m_renderTexture.resize({ fullSizeU.x, fullSizeU.y }))
            {
                std::cout << "StatDisplay's sf::RenderTexture::create failed for size=" << fullSizeU
                          << ". So the graph will show solid magenta.  The color of pure evil.\n";

                M_CHECK(m_renderTexture.resize({ 4u, 4u }), "can't create 4x4 RenderTexture");
                m_renderTexture.clear(sf::Color::Magenta);
                m_renderTexture.display();
                return;
            }

            m_renderTexture.clear(m_backgroundColor);
            m_graphDisplay.draw(graphRect.position, m_renderTexture);
            util::drawRectangleShape(m_renderTexture, borderRect, false, m_borderColor);
            m_renderTexture.draw(titleText);
            m_renderTexture.draw(subTitleText);
            m_renderTexture.display();
        }

        [[nodiscard]] const sf::Texture & texture() const { return m_renderTexture.getTexture(); }
        [[nodiscard]] const GraphDisplay<data_t> & graph() const { return m_graphDisplay; }
        [[nodiscard]] const sf::Vector2f size() const noexcept { return m_size; }
        [[nodiscard]] const std::vector<data_t> & data() const noexcept { return m_data; }

        void saveToFile(const std::string & t_filename) const
        {
            if (!m_renderTexture.getTexture().copyToImage().saveToFile(t_filename))
            {
                std::cout << "Error: StatDisplay's sf::Image::saveToFile(\"" << t_filename
                          << "\") failed.\n";
            }
        }

        static void makeAndSavePNG(
            const std::string & t_title,
            const sf::Font & t_font,
            const std::vector<data_t> & t_data,
            const sf::Vector2u & t_graphSize = { 1000, 500 })
        {
            const StatsDisplay<data_t> statsDisplay(t_title, t_font, t_data, t_graphSize);
            statsDisplay.saveToFile(t_title + ".png");
        }

        void draw(sf::RenderTarget & t_renderTarget, sf::RenderStates t_states) const override
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::fit(sprite, m_size);
            t_renderTarget.draw(sprite, t_states);
        }

        void draw(
            const sf::Vector2f & t_position,
            sf::RenderTarget & t_renderTarget,
            const sf::RenderStates t_states = {}) const
        {
            sf::Sprite sprite(m_renderTexture.getTexture());
            util::fit(sprite, m_size);
            sprite.setPosition(t_position);
            t_renderTarget.draw(sprite, t_states);
        }

      private:
        std::string m_title;
        sf::Vector2f m_size;
        std::vector<data_t> m_data;
        sf::RenderTexture m_renderTexture;
        GraphDisplay<data_t> m_graphDisplay;
        sf::Color m_backgroundColor;
        sf::Color m_borderColor;
        sf::Color m_titleTextColor;
        sf::Color m_subTitleTextColor;
    };

} // namespace util

#endif // UTIL_STATS_DISPLAY_HPP_INCLUDED
