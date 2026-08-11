//
// fly-manager.cpp
//
#include "fly-manager.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "util/check-macros.hpp"
#include "util/filesystem-util.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

namespace shadowman
{

    FlyManager::FlyManager()
        : m_flies{}
        , m_textures{}
    {
        m_flies.reserve(64); // just a guess based on what I know of the map files
    }

    void FlyManager::setup(const Context & t_context)
    {
        const std::size_t typeCount{ static_cast<std::size_t>(FlyType::Count) };
        m_textures.reserve(typeCount); // prevent any reallocation

        for (std::size_t typeIndex{ 0 }; typeIndex < typeCount; ++typeIndex)
        {
            const FlyType type{ static_cast<FlyType>(typeIndex) };
            FlyTextures & textureSet{ m_textures.emplace_back() };

            // flying animation images
            const auto flyDirPath{ t_context.setting.media_path / "image" / "fly" / toString(type) /
                                   "fly" };

            const auto flyImagePaths{ util::findFilesInDirectory(flyDirPath, ".png") };
            textureSet.fly.reserve(flyImagePaths.size()); // prevent any reallocation
            for (const auto & path : flyImagePaths)
            {
                util::TextureLoader::load(textureSet.fly.emplace_back(), path, true);
            }

            M_CHECK(
                not textureSet.fly.empty(),
                "Failed to find any flying fly images in: " << flyDirPath);

            // dying animation images
            const auto dieDirPath{ t_context.setting.media_path / "image" / "fly" / toString(type) /
                                   "die" };

            const auto dieImagePaths{ util::findFilesInDirectory(dieDirPath, ".png") };
            textureSet.die.reserve(dieImagePaths.size()); // prevent any reallocation
            for (const auto & path : dieImagePaths)
            {
                util::TextureLoader::load(textureSet.die.emplace_back(), path, true);
            }

            M_CHECK(
                not textureSet.die.empty(),
                "Failed to find any dying fly images in: " << flyDirPath);
        }
    }

    void FlyManager::add(const Context &, const sf::FloatRect &) { m_flies.emplace_back(); }

    void FlyManager::update(const Context & t_context, const float t_elapsedSec)
    {
        for (Fly & fly : m_flies)
        {
            fly.update(t_context, t_elapsedSec);
        }
    }

    void FlyManager::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (const Fly & fly : m_flies)
        {
            fly.draw(t_context, t_target, t_states);
        }
    }

} // namespace shadowman