//
// indirect-level.cpp
//
#include "indirect-level.hpp"

#include "avatar/avatar.hpp"
#include "avatar/player-info.hpp"
#include "enemy/fly-manager.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/crates.hpp"
#include "subsystem/info-region.hpp"
#include "subsystem/pickup.hpp"
#include "subsystem/screen-layout.hpp"
#include "subsystem/smoke.hpp"
#include "util/check-macros.hpp"

#include <algorithm>
#include <iostream>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace shadowman
{

    IndirectLevel::IndirectLevel()
        : m_name{}
        , m_mapTileCount{}
        , m_textureTileSize{}
        , m_screenTileSize{}
        , m_mapScreenPosOffset{}
        , m_collisions{}
        , m_lowerTileLayers{}
        , m_upperTileLayers{}
        , m_renderTexture{}
        , m_renderStates{}
        , m_offscreenTileRange{}
        , m_offscreenDrawRect{}
        , m_didOffscreenVertsChange{ false }
        , m_isMapRectBigEnoughHoriz{ false }
        , m_isMapRectBigEnoughVert{ false }
        , m_moveScreenRectLeft{}
        , m_moveScreenRectRight{}
        , m_moveScreenRectUp{}
        , m_moveScreenRectDown{}
        , m_fileLoader{}
        , m_willPreventMovingLeft{ true } // this is a right moving side-scroller after all
        , m_screenShaker{}
    {
        // harmless guesses based on what I know is in typical map files
        m_lowerTileLayers.reserve(16);
        m_upperTileLayers.reserve(16);
        m_collisions.reserve(512);
    }

    void IndirectLevel::reset(const Context & t_context)
    {
        m_screenShaker.setup(t_context);

        t_context.player_info.reset();
        t_context.pickup.clear();
        t_context.fly.clear();

        m_lowerTileLayers.clear();
        m_upperTileLayers.clear();
        m_didOffscreenVertsChange = true;

        m_collisions.clear();

        m_isMapRectBigEnoughHoriz = false;
        m_isMapRectBigEnoughVert  = false;

        const sf::FloatRect screenRect{ t_context.layout.mapRect() };
        m_mapScreenPosOffset = screenRect.position;
        m_offscreenDrawRect  = { { 0.0f, 0.0f }, screenRect.size };

        // create moveMap rects
        const sf::FloatRect innerScreenRect{ util::scaleRectInPlaceCopy(
            screenRect, t_context.setting.avatar_walk_moves_map_ratio) };

        m_moveScreenRectUp = { screenRect.position,
                               { screenRect.size.x,
                                 (screenRect.size.y - innerScreenRect.size.y) * 0.5f } };

        m_moveScreenRectDown = { { screenRect.position.x, util::bottom(innerScreenRect) },
                                 { screenRect.size.x,
                                   (screenRect.size.y - innerScreenRect.size.y) * 0.5f } };

        m_moveScreenRectLeft = { screenRect.position,
                                 { (screenRect.size.x - innerScreenRect.size.x) * 0.5f,
                                   screenRect.size.y } };

        // m_moveScreenRectRight = { { util::right(innerScreenRect), screenRect.position.y },
        //                           { (screenRect.size.x - innerScreenRect.size.x) * 0.5f,
        //                             screenRect.size.y } };

        m_moveScreenRectRight = { { util::center(innerScreenRect).x, screenRect.position.y },
                                  { innerScreenRect.size.x, screenRect.size.y } };
    }

    void IndirectLevel::setLevelDetails(
        const Context & t_context,
        const std::string & t_name,
        const sf::Vector2i & t_mapTileCount,
        const sf::Vector2i & t_textureTileSize)
    {
        m_name            = t_name;
        m_mapTileCount    = t_mapTileCount;
        m_textureTileSize = t_textureTileSize;

        const float scale{ t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.map_tile_scale) };

        m_screenTileSize = (sf::Vector2f{ m_textureTileSize } * scale);
    }

    void IndirectLevel::load(const Context & t_context, const std::string & t_filenameToLoad)
    {
        reset(t_context);
        m_fileLoader.load(t_context, t_filenameToLoad);

        const sf::Vector2f entryPos{ util::center(m_enterRect) };
        setupOffscreenTileRange(t_context, entryPos);
        t_context.avatar.setPositionOnNewLevel(t_context, entryPos);

        const sf::Vector2u renderTextureSize{ offscreenTextureSize() };
        const bool didTextureResizeSucceed{ m_renderTexture.resize(renderTextureSize) };

        M_CHECK(
            didTextureResizeSucceed,
            "IndirectLevel::load(\"" << t_filenameToLoad
                                     << "\") failed to sf::RenderTexture::resize("
                                     << renderTextureSize << ")!");

        performPostLoadSetupOnAll(t_context);
        // dumpInfo(t_filenameToLoad);

        // move map to the right because these maps all start left and move right
        while (m_offscreenTileRange.position.x < 0)
        {
            moveMapRight(t_context);
        }
    }

    void IndirectLevel::setupOffscreenTileRange(
        const Context & t_context, const sf::Vector2f & t_entryPos)
    {
        // adjustments if the map is smaller than the mapRect
        sf::Vector2i visibleTileCount{ t_context.layout.mapRect().size / m_screenTileSize };

        if (visibleTileCount.x > m_mapTileCount.x)
        {
            const float horizOffset{ (static_cast<float>(visibleTileCount.x - m_mapTileCount.x) *
                                      m_screenTileSize.x) *
                                     0.5f };

            m_mapScreenPosOffset.x += horizOffset;
            m_offscreenDrawRect.size.x -= horizOffset;

            visibleTileCount.x        = m_mapTileCount.x;
            m_isMapRectBigEnoughHoriz = true;
        }

        if (visibleTileCount.y > m_mapTileCount.y)
        {
            const float vertOffset{ (static_cast<float>(visibleTileCount.y - m_mapTileCount.y) *
                                     m_screenTileSize.y) *
                                    0.5f };

            m_mapScreenPosOffset.y += vertOffset;
            m_offscreenDrawRect.size.y -= vertOffset;

            visibleTileCount.y       = m_mapTileCount.y;
            m_isMapRectBigEnoughVert = true;
        }

        // start by centering on the center of the map
        m_offscreenTileRange =
            sf::IntRect{ ((m_mapTileCount / 2) - (visibleTileCount / 2)), visibleTileCount };

        // offset so that the entry rect is centered
        const sf::Vector2f mapCenterPos{ util::center(t_context.layout.mapRect()) };
        const sf::Vector2f entryPos{ t_entryPos + mapToScreenOffset() };
        const sf::Vector2i tilePosDiff{ (entryPos - mapCenterPos) / m_screenTileSize };

        if (!m_isMapRectBigEnoughHoriz)
        {
            m_offscreenTileRange.position.x += tilePosDiff.x;
        }

        if (!m_isMapRectBigEnoughVert)
        {
            m_offscreenTileRange.position.y += tilePosDiff.y;
        }

        // grow the offscreen drawing by two tiles in all directions
        // prevents gaps between the onscreen and offscreen area when map moves
        m_offscreenTileRange.position.x -= 2;
        m_offscreenTileRange.position.y -= 2;
        m_offscreenTileRange.size.x += 2;
        m_offscreenTileRange.size.y += 2;
    }

    void IndirectLevel::drawToOffscreenTexture(const Context & t_context)
    {
        if (m_didOffscreenVertsChange)
        {
            appendLayerVerts();
            m_didOffscreenVertsChange = false;
        }

        m_renderTexture.clear(sf::Color::Transparent);
        drawLowerLayers(m_renderTexture, m_renderStates);
        t_context.fly.draw(t_context, m_renderTexture, m_renderStates);
        t_context.smoke.draw(t_context, m_renderTexture, m_renderStates);
        t_context.info_region.draw(m_renderTexture, m_renderStates);
        t_context.crate.draw(t_context, m_renderTexture, m_renderStates);
        t_context.pickup.draw(t_context, m_renderTexture, m_renderStates);
        t_context.avatar.draw(t_context, m_renderTexture, m_renderStates);
        drawUpperLayers(m_renderTexture, m_renderStates);
        m_renderTexture.display();
    }

    void IndirectLevel::handleEvent(const Context & t_context, const sf::Event & t_event)
    {
        // TODO remove after testing
        if (const auto * keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::D)
            {
                dumpInfo(name());
            }
        }

        // if the left shift is held down then the arrow keys move which tiles are drawn
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::RShift))
        {
            return;
        }

        if (const auto * keyPtr = t_event.getIf<sf::Event::KeyPressed>())
        {
            if (keyPtr->scancode == sf::Keyboard::Scancode::Up)
            {
                if (m_offscreenTileRange.position.y > 0)
                {
                    moveMapUp(t_context);
                }
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Down)
            {
                if (util::bottom(m_offscreenTileRange) < m_mapTileCount.y)
                {
                    moveMapDown(t_context);
                }
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Left)
            {
                if (m_offscreenTileRange.position.x > 0)
                {
                    moveMapLeft(t_context);
                }
            }
            else if (keyPtr->scancode == sf::Keyboard::Scancode::Right)
            {
                if (util::right(m_offscreenTileRange) < m_mapTileCount.x)
                {
                    moveMapRight(t_context);
                }
            }
        }
    }

    void IndirectLevel::moveMapUp(const Context & t_context)
    {
        m_didOffscreenVertsChange = true;
        --m_offscreenTileRange.position.y;
        moveAll(t_context, { 0.0f, m_screenTileSize.y });
    }

    void IndirectLevel::moveMapDown(const Context & t_context)
    {
        m_didOffscreenVertsChange = true;
        ++m_offscreenTileRange.position.y;
        moveAll(t_context, { 0.0f, -m_screenTileSize.y });
    }

    void IndirectLevel::moveMapLeft(const Context & t_context)
    {
        m_didOffscreenVertsChange = true;
        --m_offscreenTileRange.position.x;
        moveAll(t_context, { m_screenTileSize.x, 0.0f });
    }

    void IndirectLevel::moveMapRight(const Context & t_context)
    {
        m_didOffscreenVertsChange = true;
        ++m_offscreenTileRange.position.x;
        moveAll(t_context, { -m_screenTileSize.x, 0.0f });
    }

    void IndirectLevel::moveAll(const Context &, const sf::Vector2f & t_move)
    {
        moveAllLayers(t_move);
    }

    void IndirectLevel::draw(
        const Context & t_context, sf::RenderTarget & t_target, sf::RenderStates t_states)
    {
        drawToOffscreenTexture(t_context);
        drawToOnscreenTexture(t_target, t_states);
    }

    void IndirectLevel::drawToOnscreenTexture(
        sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        sf::Sprite sprite(m_renderTexture.getTexture(), sf::IntRect{ m_offscreenDrawRect });
        sprite.setPosition(m_mapScreenPosOffset + m_screenShaker.offset());
        t_target.draw(sprite, t_states);
    }

    void IndirectLevel::appendLayerVerts()
    {
        for (auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->appendVerts(
                m_offscreenTileRange, m_mapTileCount, m_textureTileSize, m_screenTileSize);
        }

        for (auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->appendVerts(
                m_offscreenTileRange, m_mapTileCount, m_textureTileSize, m_screenTileSize);
        }
    }

    void IndirectLevel::dumpInfo(const std::string & t_filename) const
    {
        std::cout << "Map Graphics Layer Info for: " << t_filename << '\n';
        std::cout << "\tCollision rect count=" << m_collisions.size() << '\n';
        std::cout << "\tscreen_tile_size= " << m_screenTileSize << '\n';
        std::cout << "\ttexture_tile_size=" << m_textureTileSize << '\n';
        std::cout << "\tmap_tile_count=" << m_mapTileCount << '\n';

        for (const auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->dumpInfo();
        }

        for (const auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->dumpInfo();
        }

        std::cout << '\n';
    }

    void
        IndirectLevel::drawLowerLayers(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->draw(t_target, t_states);
        }
    }

    void
        IndirectLevel::drawUpperLayers(sf::RenderTarget & t_target, sf::RenderStates t_states) const
    {
        for (auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->draw(t_target, t_states);
        }
    }

    void IndirectLevel::update(const Context & t_context, const float t_frameTimeSec)
    {
        for (auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->update(t_context, t_frameTimeSec);
        }

        for (auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->update(t_context, t_frameTimeSec);
        }

        m_screenShaker.update(t_frameTimeSec);
    }

    void IndirectLevel::playerMove(
        const Context & t_context,
        const sf::FloatRect & t_playerMapRect,
        const sf::Vector2f & t_move)
    {
        // change to screen coordinates
        sf::FloatRect playerRect{ t_playerMapRect };
        playerRect.position += mapToScreenOffset();

        if ((t_move.x < 0.0f) and not m_isMapRectBigEnoughHoriz and not m_willPreventMovingLeft)
        {
            if (playerRect.findIntersection(m_moveScreenRectLeft).has_value())
            {
                m_offscreenDrawRect.position.x += t_move.x;

                if (m_offscreenDrawRect.position.x < std::abs(t_move.x))
                {
                    moveMapLeft(t_context);
                    m_offscreenDrawRect.position.x += m_screenTileSize.x;
                }
            }
        }
        else if ((t_move.x > 0.0f) and not m_isMapRectBigEnoughHoriz)
        {
            if (playerRect.findIntersection(m_moveScreenRectRight).has_value())
            {
                m_offscreenDrawRect.position.x += t_move.x;

                if (util::right(m_offscreenDrawRect) >
                    static_cast<float>(m_renderTexture.getSize().x))
                {
                    moveMapRight(t_context);
                    m_offscreenDrawRect.position.x -= m_screenTileSize.x;
                }
            }
        }

        if ((t_move.y < 0.0f) and not m_isMapRectBigEnoughVert)
        {
            if (playerRect.findIntersection(m_moveScreenRectUp).has_value())
            {
                m_offscreenDrawRect.position.y += t_move.y;

                if (m_offscreenDrawRect.position.y < std::abs(t_move.y))
                {
                    moveMapUp(t_context);
                    m_offscreenDrawRect.position.y += m_screenTileSize.y;
                }
            }
        }
        else if ((t_move.y > 0.0f) and not m_isMapRectBigEnoughVert)
        {
            if (playerRect.findIntersection(m_moveScreenRectDown).has_value())
            {
                m_offscreenDrawRect.position.y += t_move.y;

                if (util::bottom(m_offscreenDrawRect) >
                    static_cast<float>(m_renderTexture.getSize().y))
                {
                    moveMapDown(t_context);
                    m_offscreenDrawRect.position.y -= m_screenTileSize.y;
                }
            }
        }
    }

    void IndirectLevel::performPostLoadSetupOnAll(const Context & t_context)
    {
        for (auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->postLevelLoadSetup(t_context);
        }

        for (auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->postLevelLoadSetup(t_context);
        }
    }

    void IndirectLevel::moveAllLayers(const sf::Vector2f & t_move)
    {
        for (auto & layerUPtr : m_lowerTileLayers)
        {
            layerUPtr->move(t_move);
        }

        for (auto & layerUPtr : m_upperTileLayers)
        {
            layerUPtr->move(t_move);
        }
    }

} // namespace shadowman
