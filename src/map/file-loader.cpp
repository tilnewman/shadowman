//
// file-loader.cpp
//
#include "file-loader.hpp"

#include "map/indirect-level.hpp"
#include "map/textures.hpp"
#include "shadowman/settings.hpp"
#include "subsystem/context.hpp"
#include "subsystem/screen-layout.hpp"
#include "util/check-macros.hpp"
#include "util/sfml-util.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace shadowman
{

    FileLoader::FileLoader()
        : m_pathStr()
    {}

    void FileLoader::load(const Context & t_context, const std::string & t_filename)
    {
        const std::filesystem::path path{ t_context.setting.media_path / "map" / t_filename };
        M_CHECK(std::filesystem::exists(path), "The level file does not exist: " << path.string());

        m_pathStr = path.string();

        nlohmann::json json;

        {
            std::ifstream iStream(m_pathStr);
            M_CHECK(iStream, "Found but failed to open the level file: \"" << m_pathStr << "\"");
            iStream >> json;
        }

        parseLevelDetails(t_context, t_filename, json);
        parseTilesets(t_context, json);

        // everything else in the level file is saved in "layers"
        // which are parsed in order from back to front (lower to upper)
        parseLayers(t_context, json);
    }

    void FileLoader::parseLevelDetails(
        const Context & t_context, const std::string & t_name, const nlohmann::json & t_wholeJson)
    {
        const sf::Vector2i mapTileCount{ t_wholeJson["width"], t_wholeJson["height"] };

        M_CHECK(
            ((mapTileCount.x > 0) && (mapTileCount.y > 0)),
            "The level file had invalid t_context.level.map_tile_count values!");

        const sf::Vector2i textureTileSize{ t_wholeJson["tilewidth"], t_wholeJson["tileheight"] };

        M_CHECK(
            ((textureTileSize.x > 0) && (textureTileSize.y > 0)),
            "The level file had invalid t_context.level.texture_tile_size values!");

        t_context.level.setLevelDetails(t_context, t_name, mapTileCount, textureTileSize);
    }

    void FileLoader::parseTilesets(const Context &, const nlohmann::json & t_wholeJson)
    {
        for (const nlohmann::json & tilesetJson : t_wholeJson["tilesets"])
        {
            const int gid{ tilesetJson["firstgid"] };
            const std::string imageStr{ tilesetJson.value("image", "") };
            const std::string sourceStr{ tilesetJson.value("source", "") };

            TileImage tileImage{ TileImage::Count };
            std::string transparentColorStr{ "" };

            if (!imageStr.empty())
            {
                parseTilesetAsOrig(tilesetJson, gid, imageStr, tileImage, transparentColorStr);
            }
            else if (!sourceStr.empty())
            {
                parseTilesetAsOrig(tilesetJson, gid, sourceStr, tileImage, transparentColorStr);
            }
            else
            {
                std::cout << "FileLoader::parseTilesets()  While parsing \"" << m_pathStr
                          << "\": Found a tileset (gid=" << gid
                          << ") with no 'image' or 'source' entry.  It will be ingored.\n";
            }

            if (tileImage != TileImage::Count)
            {
                //std::cout << "tileimage=" << toString(tileImage) << ", gid=" << gid << std::endl;
                MapTextureManager::instance().setGid(tileImage, gid);
            }
        }
    }

    void FileLoader::parseTilesetAsOrig(
        const nlohmann::json & t_tilesetJson,
        const int t_gid,
        const std::string & t_path,
        TileImage & t_tileImageOutParam,
        std::string & t_transparentColorStrOutParam)
    {
        const std::filesystem::path path{ t_path };

        const std::string filenameWithoutExtension{
            path.filename().replace_extension("").string()
        };

        t_tileImageOutParam = tileImageFromString(filenameWithoutExtension);
        if (t_tileImageOutParam == TileImage::Count)
        {
            std::cout << "FileLoader::parseTilesetAsOrig()  While parsing \"" << m_pathStr
                      << "\": Found a tileset (gid=" << t_gid
                      << ") with no valid TileImage filename \"" << t_path
                      << "\".  It will be ingored.\n ";
        }
        else
        {
            t_transparentColorStrOutParam = t_tilesetJson.value("transparentcolor", "");
        }
    }

    void FileLoader::parseLayers(const Context & t_context, const nlohmann::json & jsonWholeFile)
    {
        LayerGroup layerGroup{ LayerGroup::Lower };

        for (const nlohmann::json & layerJson : jsonWholeFile["layers"])
        {
            const std::string layerName{ layerJson["name"] };
            const TileImage tileImage{ tileImageFromString(layerName) };

            if (tileImage != TileImage::Count)
            {
                parseTileLayer(t_context, layerGroup, tileImage, layerJson);
            }
            else if (layerName == "separator")
            {
                // an empty tile layer only for separating what is drawn above or below the avatar
                layerGroup = LayerGroup::Upper;
            }
            else if (layerName == "collision")
            {
                std::vector<sf::FloatRect> rects;
                rects.reserve(100);
                parseRectLayer(t_context, layerJson, rects);
                t_context.level.collisions(rects);
            }
            else if (layerName == "spawn")
            {
                parseSpawnLayer(t_context, layerJson);
            }
            else
            {
                std::cout << "FileLoader::parseLayers()  While parsing level file \"" << m_pathStr
                          << "\".  Ignored unknown layer named \"" << layerName << "\".\n";
            }
        }
    }

    void FileLoader::parseSpawnLayer(const Context & t_context, const nlohmann::json & t_json)
    {
        for (const nlohmann::json & spawnJson : t_json["objects"])
        {
            const std::string name{ spawnJson["name"] };
            const sf::FloatRect rect{ parseAndScaleRect(t_context, spawnJson) };

            if (name == "enter")
            {
                t_context.level.enterRect(rect);
            }
            else if (name == "exit")
            {
                t_context.level.exitRect(rect);
            }
            else
            {
                util::log() << "While parsing level file \"" << m_pathStr
                            << "\".  Ignored unknown spawn rect named \"" << name << "\".\n";
            }
        }

        M_CHECK(
            (t_context.level.enterRect().size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find enter location.");

        M_CHECK(
            (t_context.level.exitRect().size.x > 0.0f),
            "Error Parsing Level File " << m_pathStr << ":  Failed to find exit location.");
    }

    void FileLoader::parseTileLayer(
        const Context & t_context,
        const LayerGroup t_layerGroup,
        const TileImage image,
        const nlohmann::json & t_json)
    {
        const std::vector<int> indexes = t_json["data"];

        M_CHECK(
            !indexes.empty(),
            "Error Parsing Level File " << m_pathStr << ":  tile layer for image "
                                        << toString(image) << " was empty.");

        if (t_layerGroup == LayerGroup::Lower)
        {
            t_context.level.appendToLowerTileLayers(
                std::make_unique<TileLayer>(t_context, image, indexes));
        }
        else
        {
            t_context.level.appendToUpperTileLayers(
                std::make_unique<TileLayer>(t_context, image, indexes));
        }
    }

    void FileLoader::parseRectLayer(
        const Context & t_context,
        const nlohmann::json & t_json,
        std::vector<sf::FloatRect> & t_rects)
    {
        t_rects.clear();

        for (const nlohmann::json & collJson : t_json["objects"])
        {
            t_rects.emplace_back(parseAndScaleRect(t_context, collJson));
        }
    }

    const sf::FloatRect
        FileLoader::parseAndScaleRect(const Context & t_context, const nlohmann::json & t_json)
    {
        const sf::IntRect rect{ { t_json["x"], t_json["y"] },
                                { t_json["width"], t_json["height"] } };

        const float scale{ t_context.layout.scaleBasedOnResolution(
            t_context, t_context.setting.map_tile_scale) };

        sf::FloatRect mapRect{ rect };
        mapRect.position.x *= scale;
        mapRect.position.y *= scale;
        mapRect.size.x *= scale;
        mapRect.size.y *= scale;

        return mapRect;
    }

} // namespace shadowman
