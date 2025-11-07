#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

#include <nlohmann/json.hpp>

#include "TileMap.h"

using json = nlohmann::json;

void TileMap::loadFromJSON(const std::filesystem::path& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + jsonPath.string());
    }

    json data = json::parse(file);
    file.close();

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

bool TileMap::load(const std::filesystem::path& tilesetPath, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height)
{
    // load the tileset texture
    if (!tileset.loadFromFile(tilesetPath))
        return false;

    // resize the vertex array to fit the level size
    vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices.resize(width * height * 6);

    // populate the vertex array, with two triangles per tile
    for (unsigned int i = 0; i < width; ++i)
    {
        for (unsigned int j = 0; j < height; ++j)
        {
            // get the current tile number
            const int tileNumber = tiles[i + j * width];

            // find its position in the tileset texture
            const int tu = tileNumber % (tileset.getSize().x / tileSize.x);
            const int tv = tileNumber / (tileset.getSize().x / tileSize.x);

            // get a pointer to the triangles' vertices of the current tile
            sf::Vertex* triangles = &vertices[(i + j * width) * 6];

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

int TileMap::convert2DCoords(int row, int col) {
    return col + (row * width);
}

int TileMap::convert2DCoords(sf::Vector2i vec) {
    return vec.y + (vec.x * width);
}

int TileMap::getTileId(int tileX, int tileY) {
    //tileX and tileY start from top left 0,0
    //tileX increases going right
    //tileY increases going down
    
    int index = convert2DCoords(tileY, tileX);

    int tileId = tiles.at(index);

    return tileId;
}

int TileMap::getTileId(sf::Vector2i vec) {
    int index = convert2DCoords(vec);

    int tileId = tiles.at(index);

    return tileId;
}

bool TileMap::isLegalTile(sf::Vector2i vec) {
    int index = convert2DCoords(vec);

    int tileId = tiles.at(index);

    return tileId == 92;
}

void TileMap::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &tileset;

    // draw the vertex array
    target.draw(vertices, states);
}
