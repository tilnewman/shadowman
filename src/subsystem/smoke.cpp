//
// smoke.cpp
//
#include "smoke.hpp"

#include "map/indirect-level.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/check-macros.hpp"
#include "util/filesystem-util.hpp"
#include "util/random.hpp"
#include "util/sfml-util.hpp"
#include "util/sound-player.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include <filesystem>

namespace shadowman
{

    SmokeDetails::SmokeDetails(const std::string & t_details)
        : type{ SmokeType::Full } // any default works here
        , alpha{ 0 }              // any default works here too
    {
        std::string typeStr;
        std::string alphaStr;

        bool hasReachedComma{ false };
        for (const char ch : t_details)
        {
            if (',' == ch)
            {
                hasReachedComma = true;
                continue;
            }

            if (hasReachedComma)
            {
                alphaStr += ch;
            }
            else
            {
                typeStr += ch;
            }
        }

        if (("full" == typeStr) || ("Full" == typeStr))
        {
            type = SmokeType::Full;
        }
        else if (("still" == typeStr) || ("Still" == typeStr))
        {
            type = SmokeType::Still;
        }
        else if (("top" == typeStr) || ("Top" == typeStr))
        {
            type = SmokeType::Top;
        }
        else
        {
            M_LOG(
                "Unable to parse smoke details \"" << t_details
                                                   << "\" into a \"type,alpha\" string.");
        }

        alpha = static_cast<std::uint8_t>(std::atoi(alphaStr.c_str()));
    }

    //

    SmokeManager::SmokeManager()
        : m_textures{}
        , m_animations{}
    {
        // probably only half a dozen in any typical map
        m_animations.reserve(16);
    }

    void SmokeManager::setup(const Context & t_context)
    {
        const auto smokeImageDirPath{ t_context.setting.media_path / "image" / "smoke" };
        const auto paths{ util::findFilesInDirectory(smokeImageDirPath, ".png") };

        for (const auto & path : paths)
        {
            util::TextureLoader::load(m_textures.emplace_back(), path, true);
        }

        M_CHECK(
            !m_textures.empty(),
            "Failed to find/load any smoke particle images at: " << smokeImageDirPath.string());
    }

    void SmokeManager::add(
        const Context & t_context, const sf::FloatRect & t_region, const std::string & t_details)
    {
        const std::size_t frameIndex{ t_context.random.zeroToOneLessThan(m_textures.size()) };
        sf::Sprite sprite{ m_textures.at(frameIndex) };
        util::scaleAndCenterInside(sprite, t_region);

        const bool isFacingRight{ t_context.random.boolean() };
        if (!isFacingRight)
        {
            sprite.scale({ -1.0f, 1.0f });
            sprite.move({ sprite.getGlobalBounds().size.x, 0.0f });
        }

        const SmokeDetails details{ t_details };

        sf::Color smokeColor{ t_context.setting.smoke_color };
        smokeColor.a = details.alpha;
        sprite.setColor(smokeColor);

        m_animations.emplace_back(details.type, sprite, frameIndex, t_region, isFacingRight);
    }

    void SmokeManager::update(const Context & t_context, const float t_elapsedTimeSec)
    {
        const float timeBetweenFramesSec{ t_context.setting.smoke_animation_time_between_frames };

        for (SmokeAnim & anim : m_animations)
        {
            if (SmokeType::Still == anim.type)
            {
                continue;
            }

            anim.elapsed_time_sec += t_elapsedTimeSec;
            if (anim.elapsed_time_sec > timeBetweenFramesSec)
            {
                anim.elapsed_time_sec -= timeBetweenFramesSec;

                if (++anim.frame_index >= m_textures.size())
                {
                    anim.frame_index = 0;
                }

                anim.sprite.setTexture(m_textures.at(anim.frame_index));

                if (SmokeType::Top == anim.type)
                {
                    const sf::IntRect offscreenRect(
                        { 0, 0 }, { static_cast<int>(m_textures.at(0).getSize().x), 88 });

                    anim.sprite.setTextureRect(offscreenRect);
                    util::scaleAndCenterInside(anim.sprite, anim.rect);

                    if (!anim.is_facing_right)
                    {
                        anim.sprite.scale({ -1.0f, 1.0f });
                        anim.sprite.move({ anim.sprite.getGlobalBounds().size.x, 0.0f });
                    }
                }
            }
        }
    }

    void SmokeManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        const sf::Vector2f offset{ t_context.level.mapToOffscreenOffset() };
        for (const SmokeAnim & anim : m_animations)
        {
            sf::Sprite tempSprite{ anim.sprite };
            tempSprite.move(offset);
            t_target.draw(tempSprite, t_states);
        }
    }

    void SmokeManager::move(const sf::Vector2f & t_move)
    {
        for (SmokeAnim & anim : m_animations)
        {
            anim.sprite.move(t_move);
            anim.rect.position += t_move;
        }
    }

} // namespace shadowman
