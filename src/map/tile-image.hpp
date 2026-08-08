#ifndef MAP_TILE_IMAGE_HPP_INCLUDED
#define MAP_TILE_IMAGE_HPP_INCLUDED
//
// tile-image.hpp
//
#include <string>
#include <string_view>

namespace shadowman
{

    enum class TileImage : unsigned char
    {
        Platform,
        Count
    };

    constexpr std::string_view toString(const TileImage t_image) noexcept
    {
        // clang-format off
        switch (t_image)
        {
            case TileImage::Platform: { return "platform"; }
            case TileImage::Count:
            default:    { return "Error_TileImage_Unknown"; }
        }
        // clang-format on
    }

    inline TileImage tileImageFromString(const std::string & t_filename) noexcept
    {
        // clang-format off
        if (t_filename == "platform") { return TileImage::Platform; }
        else                          { return TileImage::Count;    }
        // clang-format on
    }

    inline std::string toTileImageFilename(const TileImage t_image)
    {
        std::string filename{ toString(t_image) };
        filename += ".png";
        return filename;
    }

} // namespace shadowman

#endif // MAP_TILE_IMAGE_HPP_INCLUDED
