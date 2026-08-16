// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
// text-layout.cpp
//
#include "text-layout.hpp"

#include "subsystem/context.hpp"
#include "subsystem/font.hpp"
#include "util/sfml-util.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>

namespace shadowman
{

    const TextLayoutPack TextLayout::typeset(
        const Context & t_context,
        const std::string & t_text,
        const sf::FloatRect & t_rectOrig,
        const TextDetails & t_textDetails)
    {
        const FontExtent fontExtent{ t_context.font.extent(
            t_textDetails.font, t_textDetails.size) };

        // split message into words
        const std::vector<std::string> words{ splitIntoWords(t_text) };

        // transform words into one sf::Text per line of text that fits into t_rect
        TextLayoutPack layout;
        layout.rect_orig = t_rectOrig;

        layout.rect_padded =
            util::scaleRectInPlaceCopy(t_rectOrig, (1.0f - t_textDetails.inner_pad_ratio));

        if (words.empty())
        {
            return layout;
        }

        layout.texts.reserve(words.size());

        sf::Vector2f pos{ layout.rect_padded.position };
        std::string lineStr;
        sf::Text lineText = t_context.font.makeText("", t_textDetails);
        lineText.setPosition(pos);

        for (auto wordIter = std::begin(words); wordIter != std::end(words); ++wordIter)
        {
            if ("<p>" == *wordIter)
            {
                layout.texts.push_back(lineText);
                pos.y += fontExtent.letter_size.y;
                pos.y += fontExtent.letter_size.y;
                lineStr.clear();
                lineText.setString(lineStr);
                util::setOriginToPosition(lineText);
                lineText.setPosition(pos);
                continue;
            }

            std::string tempStr;
            if (lineStr.empty())
            {
                tempStr = *wordIter;
            }
            else
            {
                tempStr = lineStr;
                tempStr += ' ';
                tempStr += *wordIter;
            }

            sf::Text tempText{ lineText };
            tempText.setString(tempStr);
            util::setOriginToPosition(tempText);
            tempText.setPosition(pos);

            if (util::right(tempText) < util::right(layout.rect_padded))
            {
                lineText = tempText;
                lineStr  = tempStr;
            }
            else
            {
                layout.texts.push_back(lineText);

                pos.y += fontExtent.letter_size.y;
                lineStr = *wordIter;
                lineText.setString(lineStr);
                util::setOriginToPosition(lineText);
                lineText.setPosition(pos);
            }
        }

        layout.texts.push_back(lineText);

        // center if needed
        if (t_textDetails.will_center_horiz || t_textDetails.will_center_vert)
        {
            centerText(layout, t_textDetails.will_center_horiz, t_textDetails.will_center_vert);
        }

        // establish rect_actual
        float left{ std::numeric_limits<float>::max() };
        float right{ 0.0f };
        for (const sf::Text & text : layout.texts)
        {
            left  = std::min(text.getPosition().x, left);
            right = std::max(util::right(text), right);
        }
        layout.rect_actual.position.x = left;
        layout.rect_actual.position.y = layout.texts.front().getPosition().y;
        layout.rect_actual.size.x     = (right - left);
        
        layout.rect_actual.size.y =
            (util::bottom(layout.texts.back()) - layout.rect_actual.position.y);

        return layout;
    }

    const std::vector<std::string> TextLayout::splitIntoWords(const std::string & t_text)
    {
        std::vector<std::string> words;
        words.reserve(t_text.size() / 4); // found by experiment to be a good upper bound

        std::istringstream iss{ t_text };

        std::copy(
            std::istream_iterator<std::string>(iss),
            std::istream_iterator<std::string>(),
            std::back_inserter(words));

        return words;
    }

    void TextLayout::centerText(
        TextLayoutPack & layout, const bool t_willCenterHoriz, const bool t_willCenterVert)
    {
        if (layout.texts.empty())
        {
            return;
        }

        const float height{ util::bottom(layout.texts.back()) -
                            layout.texts.front().getGlobalBounds().position.y };

        for (sf::Text & text : layout.texts)
        {
            sf::Vector2f offset{ 0.0, 0.0f };

            if (t_willCenterHoriz)
            {
                offset.x = ((layout.rect_padded.size.x - text.getGlobalBounds().size.x) * 0.5f);
            }

            if (t_willCenterVert)
            {
                offset.y = ((layout.rect_padded.size.y - height) * 0.5f);
            }

            text.move(offset);
        }
    }

} // namespace bramblefore
