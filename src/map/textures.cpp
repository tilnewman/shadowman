//
// textures.cpp
//
#include "textures.hpp"

#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "util/check-macros.hpp"
#include "util/texture-loader.hpp"

#include <SFML/Graphics/Image.hpp>

namespace shadowman
{

    MapTextureManager::MapTextureManager()
        : m_tileTextures{}
    {}

    MapTextureManager & MapTextureManager::instance()
    {
        static MapTextureManager instance;
        return instance;
    }

    void MapTextureManager::setup()
    {
        // size only this once and prevent any resizing/reallocations except for teardown()
        m_tileTextures.resize(static_cast<std::size_t>(TileImage::Count));
    }

    void MapTextureManager::teardown()
    {
        for (std::size_t imageIndex{ 0 }; imageIndex < static_cast<std::size_t>(TileImage::Count);
             ++imageIndex)
        {
            const TileTexture & tileTexture{ m_tileTextures.at(imageIndex) };

            M_CHECK(
                (0 == tileTexture.ref_count),
                toString(static_cast<TileImage>(imageIndex))
                    << "'s ref_count!=0 when teardown() was called!  It was="
                    << tileTexture.ref_count);
        }

        m_tileTextures = std::vector<TileTexture>();
    }

    void MapTextureManager::acquire(const Context & t_context, const TileImage t_tileImage)
    {
        const std::size_t imageIndex{ static_cast<std::size_t>(t_tileImage) };

        M_CHECK(
            (imageIndex < m_tileTextures.size()),
            "TileImage " << toString(t_tileImage) << " of index " << imageIndex
                         << " >= " << m_tileTextures.size() << " -maybe setup() was not called?");

        TileTexture & tileTexture{ m_tileTextures.at(imageIndex) };

        if (0 == tileTexture.ref_count)
        {
            const auto path{ t_context.setting.media_path / "image" / "map" /
                             toTileImageFilename(t_tileImage) };

            util::TextureLoader::load(tileTexture.texture, path);
        }

        ++tileTexture.ref_count;
    }

    void MapTextureManager::release(const TileImage t_tileImage)
    {
        const std::size_t imageIndex{ static_cast<std::size_t>(t_tileImage) };

        M_CHECK(
            (imageIndex < m_tileTextures.size()),
            "TileImage " << toString(t_tileImage) << " of index " << imageIndex
                         << " >= " << m_tileTextures.size() << " -maybe setup() was not called?");

        TileTexture & tileTexture{ m_tileTextures.at(imageIndex) };

        M_CHECK(
            (0 != tileTexture.ref_count),
            toString(t_tileImage) << "'s ref_count was already zero.");

        if (1 == tileTexture.ref_count)
        {
            tileTexture.texture = sf::Texture();
        }

        --tileTexture.ref_count;
    }

    const TileTexture & MapTextureManager::get(const TileImage t_tileImage) const
    {
        const std::size_t imageIndex{ static_cast<std::size_t>(t_tileImage) };

        M_CHECK(
            (imageIndex < m_tileTextures.size()),
            "TileImage " << toString(t_tileImage) << " of index " << imageIndex
                         << " >= " << m_tileTextures.size() << " -maybe setup() was not called?");

        return m_tileTextures.at(imageIndex);
    }

    void MapTextureManager::setGid(const TileImage t_tileImage, const int t_gid)
    {
        const std::size_t imageIndex{ static_cast<std::size_t>(t_tileImage) };

        M_CHECK(
            (imageIndex < m_tileTextures.size()),
            "TileImage " << toString(t_tileImage) << " of index " << imageIndex
                         << " >= " << m_tileTextures.size() << " -maybe setup() was not called?");

        m_tileTextures.at(imageIndex).gid = t_gid;
    }

} // namespace shadowman
