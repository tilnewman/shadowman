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
        Platform = 0,
        Platform2,
        Platform3,
        Structure,
        Rock,
        Misc,
        Count
    };

    constexpr std::string_view toString(const TileImage t_image) noexcept
    {
        // clang-format off
        switch (t_image)
        {
            case TileImage::Platform:  { return "platform";  }
            case TileImage::Platform2: { return "platform2"; }
            case TileImage::Platform3: { return "platform3"; }
            case TileImage::Structure: { return "structure"; }
            case TileImage::Rock:      { return "rock";      }
            case TileImage::Misc:      { return "misc";      }
            case TileImage::Count:
            default:     { return "Error_TileImage_Unknown"; }
        }
        // clang-format on
    }

    inline TileImage tileImageFromString(const std::string & t_filename) noexcept
    {
        // clang-format off
        if      (t_filename == "platform")  { return TileImage::Platform;  }
        else if (t_filename == "platform2") { return TileImage::Platform2; }
        else if (t_filename == "platform3") { return TileImage::Platform3; }
        else if (t_filename == "structure") { return TileImage::Structure; }
        else if (t_filename == "rock")      { return TileImage::Rock;      }
        else if (t_filename == "misc")      { return TileImage::Misc;      }
        else                                { return TileImage::Count;     }
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
