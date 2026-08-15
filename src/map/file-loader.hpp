#ifndef MAP_FILE_LOADER_HPP_INCLUDED
#define MAP_FILE_LOADER_HPP_INCLUDED
//
// file-loader.hpp
//
#include "map/json-wrapper.hpp"
#include "map/tile-image.hpp"
#include "map/tile-layer.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <string>
#include <vector>

namespace shadowman
{
    struct Context;

    enum class LayerGroup : unsigned char
    {
        Upper,
        Lower
    };

    class FileLoader
    {
      public:
        FileLoader();

        void load(const Context & t_context, const std::string & t_filename);

      private:
        void parseLevelDetails(
            const Context & t_context,
            const std::string & t_name,
            const nlohmann::json & t_wholeJson);

        void parseTilesets(const Context & t_context, const nlohmann::json & t_wholeJson);

        void parseTilesetAsOrig(
            const nlohmann::json & t_json,
            const int t_gid,
            const std::string & t_path,
            TileImage & t_tileImageOutParam,
            std::string & t_transparentColorStrOutParam);

        void parseLayers(const Context & t_context, const nlohmann::json & t_json);
        void parseTeleportLayer(const Context & t_context, const nlohmann::json & t_json);
        void parseEnemyLayer(const Context & t_context, const nlohmann::json & t_json);

        void parseTileLayer(
            const Context & t_context,
            const LayerGroup t_layerGroup,
            const TileImage image,
            const nlohmann::json & t_json);

        void parseRectLayer(
            const Context & t_context,
            const nlohmann::json & t_json,
            std::vector<sf::FloatRect> & t_rects);

        [[nodiscard]] const sf::FloatRect
            parseAndScaleRect(const Context & t_context, const nlohmann::json & t_json) const;

        void parseSmokeLayer(const Context & t_context, const nlohmann::json & t_json) const;
        void parsePickupLayer(const Context & t_context, const nlohmann::json & t_json) const;

      private:
        std::string m_pathStr;
    };

} // namespace shadowman

#endif // MAP_FILE_LOADER_HPP_INCLUDED
