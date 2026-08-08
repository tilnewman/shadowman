#ifndef MAP_TILE_LAYER_HPP_INCLUDED
#define MAP_TILE_LAYER_HPP_INCLUDED
//
// tile-layer.hpp
//
#include "tile-image.hpp"

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Vertex.hpp>

#include <vector>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{
    struct Context;

    // TileLayers are heavywight classes that manage the big load/unload of map tile images
    // in their constructor/destructor and store their tiles as vert pairs.  Each of these
    // tile layers is directly loaded from and correlates to the mapping software's "layers".
    // Each of these tile layers is the whole map in size.
    struct ITileLayer
    {
        virtual ~ITileLayer() = default;

        virtual void postLevelLoadSetup(const Context & t_context)                      = 0;
        virtual void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const = 0;
        virtual void dumpInfo() const                                                   = 0;
        virtual void update(const Context & t_context, const float t_elapsedSec)        = 0;
        virtual void move(const sf::Vector2f & t_move)                                  = 0;

        virtual void appendVerts(
            const sf::IntRect & t_mapTileRange,
            const sf::Vector2i & t_mapTileCount,
            const sf::Vector2i & t_textureTileSize,
            const sf::Vector2f & t_screenTileSize) = 0;
    };

    struct MapTile
    {
        MapTile(const sf::Vector2i & t_position, const sf::IntRect & t_textureRect)
            : position{ t_position }
            , texture_rect{ t_textureRect }
        {}

        sf::Vector2i position{};
        sf::IntRect texture_rect{};
    };

    class TileLayer : public ITileLayer
    {
      public:
        TileLayer(
            const Context & t_context, const TileImage t_image, const std::vector<int> & t_indexes);

        ~TileLayer() override;

        void postLevelLoadSetup(const Context &) override {}
        void draw(sf::RenderTarget & t_target, sf::RenderStates t_states) const override;
        void dumpInfo() const override;
        void update(const Context &, const float) override {}
        void move(const sf::Vector2f &) override {}

        void appendVerts(
            const sf::IntRect & t_mapTileRange,
            const sf::Vector2i & t_mapTileCount,
            const sf::Vector2i & t_textureTileSize,
            const sf::Vector2f & t_screenTileSize) override;

      private:
        // version 1: translates map indexes into verts that get drawn
        // this original version is not in use, but will likely need to be referenced
        // void appendVertsRaw(
        //     const sf::IntRect & t_mapTileRange,
        //     const sf::Vector2i & t_mapTileCount,
        //     const sf::Vector2i & t_textureTileSize,
        //     const sf::Vector2f & t_screenTileSize);

        // version 2: translates the reduced indexes in m_mapTiles into verts that get drawn
        void appendVertsReduced(
            const sf::IntRect & t_mapTileRange, const sf::Vector2f & t_screenTileSize);

        void createReducedMapTiles(
            const sf::Vector2i & t_mapTileCount, const sf::Vector2i & t_textureTileSize);

      private:
        TileImage m_image;

        // used by the map authoring software (Tiled)
        int m_textureGID;

        // size of the layer/texture/image in map tiles
        sf::Vector2i m_textureTileCount;

        // one int for each map tile in xy order over the whole map
        std::vector<int> m_rawIndexes;

        // optimized version of the m_rawIndexes
        std::vector<MapTile> m_mapTiles;

        // minially complete verts to be drawn for this layer
        std::vector<sf::Vertex> m_verts;
    };

} // namespace shadowman

#endif // MAP_TILE_LAYER_HPP_INCLUDED
