#ifndef MAP_TEXTURES_HPP_INCLUDED
#define MAP_TEXTURES_HPP_INCLUDED
//
// textures.hpp
//
#include "tile-image.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <vector>

namespace shadowman
{

    struct Context;

    struct TileTexture
    {
        std::size_t ref_count{ 0 };
        int gid{ 0 };
        sf::Texture texture{};
    };

    class MapTextureManager
    {
      public:
        MapTextureManager();

        // this class MUST be a singleton because it needs to be accessible in TileLayer destructors
        // TODO but then this could just be a static member of TileLayer...right?
        static MapTextureManager & instance();

        void setup();
        void teardown();

        void acquire(const Context & t_context, const TileImage t_tileImage);
        void release(const TileImage t_tileImage);

        [[nodiscard]] const TileTexture & get(const TileImage t_tileImage) const;
        void setGid(const TileImage t_tileImage, const int t_gid);

      private:
        std::vector<TileTexture> m_tileTextures;
    };

} // namespace thornberry

#endif // MAP_TEXTURES_HPP_INCLUDED
