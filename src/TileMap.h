#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include <SFML/Graphics.hpp>

using json = nlohmann::json;

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    std::vector<int> tiles;
    unsigned int width;
    unsigned int height;
    unsigned int tileSize;

    void loadFromJSON(const std::filesystem::path& jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + jsonPath.string());
        }

        json data = json::parse(file);
        file.close();

        //TileMapData result;
        //result.tileSize = data["tileSize"].get<unsigned int>();

        tileSize = data["tileSize"].get<unsigned int>();

        // Get the first layer's tiles
        if (data["layers"].empty() || data["layers"][0]["tiles"].empty()) {
            throw std::runtime_error("No tiles found in JSON");
        }

        const auto& tilesArray = data["layers"][0]["tiles"];

        // Find the bounds of the tilemap
        int minX = std::numeric_limits<int>::max();
        int minY = std::numeric_limits<int>::max();
        int maxX = std::numeric_limits<int>::min();
        int maxY = std::numeric_limits<int>::min();

        for (const auto& tile : tilesArray) {
            int x = tile["x"].get<int>();
            int y = tile["y"].get<int>();
            minX = std::min(minX, x);
            minY = std::min(minY, y);
            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
        }

        // Calculate grid dimensions
        width = (maxX - minX) / tileSize + 1;
        height = (maxY - minY) / tileSize + 1;

        // Initialize the grid with zeros (empty tiles)
        tiles.resize(width * height, 0);

        // Fill in the tiles from the JSON
        for (const auto& tile : tilesArray) {
            int x = tile["x"].get<int>();
            int y = tile["y"].get<int>();
            int tileId = std::stoi(tile["id"].get<std::string>());

            // Convert pixel position to grid position
            int gridX = (x - minX) / tileSize;
            int gridY = (y - minY) / tileSize;

            // Store in the grid
            tiles[gridX + gridY * width] = tileId;
        }
    }

    bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
    {
        // load the tileset texture
        if (!m_tileset.loadFromFile(tileset))
            return false;

        // resize the vertex array to fit the level size
        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_vertices.resize(width * height * 6);

        // populate the vertex array, with two triangles per tile
        for (unsigned int i = 0; i < width; ++i)
        {
            for (unsigned int j = 0; j < height; ++j)
            {
                // get the current tile number
                const int tileNumber = tiles[i + j * width];

                // find its position in the tileset texture
                const int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                const int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                // get a pointer to the triangles' vertices of the current tile
                sf::Vertex* triangles = &m_vertices[(i + j * width) * 6];

                // define the 6 corners of the two triangles
                triangles[0].position = sf::Vector2f(i * tileSize.x, j * tileSize.y);
                triangles[1].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                triangles[2].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
                triangles[3].position = sf::Vector2f(i * tileSize.x, (j + 1) * tileSize.y);
                triangles[4].position = sf::Vector2f((i + 1) * tileSize.x, j * tileSize.y);
                triangles[5].position = sf::Vector2f((i + 1) * tileSize.x, (j + 1) * tileSize.y);

                // define the 6 matching texture coordinates
                triangles[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
                triangles[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                triangles[2].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
                triangles[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
                triangles[4].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
                triangles[5].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
            }
        }

        return true;
    }

    int convert2DCoords(int row, int col) {
        return col + (row * width);
    }

    int convert2DCoords(sf::Vector2i vec) {
        return vec.y + (vec.x * width);
    }

    int getTileId(int tileX, int tileY) {
        //tileX and tileY start from top left 0,0
        //tileX increases going right
        //tileY increases going down
        
        int index = convert2DCoords(tileY, tileX);

        int tileId = tiles.at(index);

        return tileId;
    }

    int getTileId(sf::Vector2i vec) {
        int index = convert2DCoords(vec);

        int tileId = tiles.at(index);

        return tileId;
    }

    bool isLegalTile(sf::Vector2i vec) {
        int index = convert2DCoords(vec);

        int tileId = tiles.at(index);

        return tileId == 92;
    }


private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        // apply the transform
        states.transform *= getTransform();

        // apply the tileset texture
        states.texture = &m_tileset;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture     m_tileset;
};

#endif
