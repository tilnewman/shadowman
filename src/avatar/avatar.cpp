//
// avatar.cpp
//
#include "avatar.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/filesystem-util.hpp"
#include "util/sfml-defaults.hpp"
#include "util/sfml-util.hpp"
#include "util/texture-loader.hpp"

#include <filesystem>

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    Avatar::Avatar()
        : m_anim{ AvatarAnim::Jump }
        , m_sprite{ util::SfmlDefaults::instance().texture() }
        , m_animElapsedSec{ 0.0f }
        , m_frameIndex{ 0 }
        , m_animTextures{}
    {}

    void Avatar::setup(const Context & t_context)
    {
        // load all textures
        const std::size_t animCount{ static_cast<std::size_t>(AvatarAnim::Count) };
        m_animTextures.reserve(animCount);

        for (std::size_t animIndex{ 0 }; animIndex < animCount; ++animIndex)
        {
            const AvatarAnim anim{ static_cast<AvatarAnim>(animIndex) };
            std::vector<sf::Texture> & textures{ m_animTextures.emplace_back() };

            const auto dirPath{ t_context.setting.media_path / "image" / "avatar" /
                                toString(anim) };

            const auto imagePaths{ util::findFilesInDirectory(dirPath, ".png") };

            M_CHECK(not imagePaths.empty(), "Failed to find any PNG image files in: " << dirPath);

            for (const auto & path : imagePaths)
            {
                util::TextureLoader::load(textures.emplace_back(), path, true);
            }
        }

        // setup sprite
        m_sprite.setTexture(m_animTextures.at(static_cast<std::size_t>(m_anim)).at(0), true);

        const float scale{ t_context.setting.avatar_scale };
        m_sprite.scale({ scale, scale });

        util::centerInside(m_sprite, t_context.layout.wholeRect());
    }

    void Avatar::update(const Context &, const float t_elapsedSec)
    {
        m_animElapsedSec += t_elapsedSec;
        const float timePerFrameSec{ 0.0008f };
        if (m_animElapsedSec > timePerFrameSec)
        {
            m_animElapsedSec -= timePerFrameSec;

            if (++m_frameIndex >= m_animTextures.at(static_cast<std::size_t>(m_anim)).size())
            {
                m_frameIndex = 0;

                std::size_t animIndex{ static_cast<std::size_t>(m_anim) };
                if (++animIndex >= static_cast<std::size_t>(AvatarAnim::Count))
                {
                    animIndex = 0;
                }

                m_anim = static_cast<AvatarAnim>(animIndex);
            }

            m_sprite.setTexture(
                m_animTextures.at(static_cast<std::size_t>(m_anim)).at(m_frameIndex), true);
        }
    }

    void Avatar::draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        t_target.draw(m_sprite, t_states);
    }

} // namespace shadowman
