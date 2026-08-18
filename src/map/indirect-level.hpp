#ifndef MAP_INDIRECT_LEVEL_HPP_INCLUDED
#define MAP_INDIRECT_LEVEL_HPP_INCLUDED
//
// indirect-level.hpp
//
#include "map/file-loader.hpp"
#include "map/tile-layer.hpp"
#include "subsystem/screen-shaker.hpp"
#include "util/sfml-util.hpp"

#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Window/Event.hpp>

namespace sf
{
    class RenderTarget;
}

namespace shadowman
{
    struct Context;

    // This class (and other classes) are called "indirect" because they are not directly drawn
    // to the screen, but rather drawn to an offscreen texture.
    class IndirectLevel
    {
      public:
        IndirectLevel();

        void load(const Context & t_context, const std::string & t_filenameToLoad);
        void update(const Context & t_context, const float t_frameTimeSec);
        void handleEvent(const Context & t_context, const sf::Event & t_event);

        void moveMapUp(const Context & t_context);
        void moveMapDown(const Context & t_context);
        void moveMapLeft(const Context & t_context);
        void moveMapRight(const Context & t_context);

        void
            draw(const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states);

        [[nodiscard]] inline const std::string name() const { return m_name; }
        [[nodiscard]] inline const sf::Vector2i mapTileCount() const { return m_mapTileCount; }

        [[nodiscard]] inline const sf::Vector2i textureTileSize() const
        {
            return m_textureTileSize;
        }

        [[nodiscard]] inline const sf::Vector2f screenTileSize() const { return m_screenTileSize; }

        [[nodiscard]] inline const sf::Vector2f mapScreenPosOffset() const
        {
            return m_mapScreenPosOffset;
        }

        [[nodiscard]] inline const sf::FloatRect offscreenRect() const
        {
            return m_offscreenDrawRect;
        }

        [[nodiscard]] inline const std::vector<sf::FloatRect> & collisions() const
        {
            return m_collisions;
        }

        inline void collisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_collisions = t_rects;
        }

        [[nodiscard]] inline const sf::FloatRect enterRect() const { return m_enterRect; }
        inline void enterRect(const sf::FloatRect & t_rect) { m_enterRect = t_rect; }

        [[nodiscard]] inline const sf::FloatRect exitRect() const { return m_exitRect; }
        inline void exitRect(const sf::FloatRect & t_rect) { m_exitRect = t_rect; }

        [[nodiscard]] inline const std::vector<sf::FloatRect> & killCollisions() const
        {
            return m_killCollisions;
        }

        inline void killCollisions(const std::vector<sf::FloatRect> & t_rects)
        {
            m_killCollisions = t_rects;
        }

        [[nodiscard]] inline const std::vector<sf::FloatRect> & acidRects() const
        {
            return m_acidRects;
        }

        inline void acidRects(const std::vector<sf::FloatRect> & t_rects)
        {
            m_acidRects = t_rects;
        }

        inline void appendToLowerTileLayers(std::unique_ptr<ITileLayer> t_uptr)
        {
            m_lowerTileLayers.emplace_back(std::move(t_uptr));
        }

        inline void appendToUpperTileLayers(std::unique_ptr<ITileLayer> t_uptr)
        {
            m_upperTileLayers.emplace_back(std::move(t_uptr));
        }

        [[nodiscard]] inline const sf::Vector2f mapToOffscreenOffset() const
        {
            return (sf::Vector2f{ m_offscreenTileRange.position } * m_screenTileSize * -1.0f);
        }

        [[nodiscard]] inline const sf::Vector2f mapToScreenOffset() const
        {
            return (m_mapScreenPosOffset + mapToOffscreenOffset());
        }

        // the map loader calls this to set things up very early in the process of loading
        void setLevelDetails(
            const Context & t_context,
            const std::string & t_name,
            const sf::Vector2i & t_mapTileCount,
            const sf::Vector2i & t_textureTileSize);

        // returns true if the avatar will move by t_move amount
        void playerMove(
            const Context & t_context,
            const sf::FloatRect & t_playerMapRect,
            const sf::Vector2f & t_move);

        constexpr void shakeScreen(const Context & t_context, const bool m_isEnabled)
        {
            if (m_isEnabled)
            {
                m_screenShaker.start();
            }
            else
            {
                m_screenShaker.stop();
                m_screenShaker.setup(t_context);
            }
        }

      private:
        void setupOffscreenTileRange(const Context & t_context, const sf::Vector2f & t_entryPos);
        void reset(const Context & t_context);
        void appendLayerVerts();
        void dumpInfo(const std::string & t_filename) const;
        void drawLowerLayers(sf::RenderTarget & t_target, sf::RenderStates t_states) const;
        void drawUpperLayers(sf::RenderTarget & t_target, sf::RenderStates t_states) const;
        void drawToOffscreenTexture(const Context & t_context);
        void drawToOnscreenTexture(sf::RenderTarget & t_target, sf::RenderStates t_states) const;

        [[nodiscard]] const sf::Vector2f
            findEntryTransitionPositionFrom(const std::string & t_fromFilename) const;

        void performPostLoadSetupOnAll(const Context & t_context);
        void moveAllLayers(const sf::Vector2f & t_move);
        void moveAll(const Context & t_context, const sf::Vector2f & t_move);

        [[nodiscard]] inline const sf::Vector2f offscreenTextureSize() const
        {
            return (m_screenTileSize * sf::Vector2f{ m_offscreenTileRange.size });
        }

      private:
        std::string m_name;

        sf::Vector2i m_mapTileCount;       // how big is the whole map that was loaded
        sf::Vector2i m_textureTileSize;    // offscreen tile size
        sf::Vector2f m_screenTileSize;     // onscreen tile size
        sf::Vector2f m_mapScreenPosOffset; // map to onscreen position offset

        // all of these are in map coordinates
        std::vector<sf::FloatRect> m_collisions;
        std::vector<sf::FloatRect> m_killCollisions;
        sf::FloatRect m_enterRect;
        sf::FloatRect m_exitRect;
        std::vector<sf::FloatRect> m_acidRects;

        // lower layers draw first, then pre anims, then player/nps, then upper layers, post anims
        std::vector<std::unique_ptr<ITileLayer>> m_lowerTileLayers;
        std::vector<std::unique_ptr<ITileLayer>> m_upperTileLayers;

        // the offscreen texture
        sf::RenderTexture m_renderTexture;
        sf::RenderStates m_renderStates;

        // which map tiles will be drawn offscreen
        sf::IntRect m_offscreenTileRange;
        sf::FloatRect m_offscreenDrawRect;

        // dirty flag for the offscreen verts, so we don't have to re-append them every frame
        bool m_didOffscreenVertsChange;

        // lots of special cases to handle when the offscreen map fits horiz/vert onscreen
        bool m_isMapRectBigEnoughHoriz;
        bool m_isMapRectBigEnoughVert;

        // these rects trigger movement of the map when the avatar walks on them
        sf::FloatRect m_moveScreenRectLeft;
        sf::FloatRect m_moveScreenRectRight;
        sf::FloatRect m_moveScreenRectUp;
        sf::FloatRect m_moveScreenRectDown;

        FileLoader m_fileLoader;
        bool m_willPreventMovingLeft;
        ScreenShaker m_screenShaker;
    };

} // namespace shadowman

#endif // MAP_INDIRECT_LEVEL_HPP_INCLUDED
