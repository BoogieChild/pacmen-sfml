#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;

struct TilemapData {
    std::vector<int> tiles;
    unsigned int width;
    unsigned int height;
    unsigned int tileSize;
};

class TilemapLoader {
public:
    static TilemapData loadFromJSON(const std::filesystem::path& jsonPath) {
        std::ifstream file(jsonPath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open JSON file: " + jsonPath.string());
        }

        json data = json::parse(file);
        file.close();

        TilemapData result;
        result.tileSize = data["tileSize"].get<unsigned int>();

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
        result.width = (maxX - minX) / result.tileSize + 1;
        result.height = (maxY - minY) / result.tileSize + 1;

        // Initialize the grid with zeros (empty tiles)
        result.tiles.resize(result.width * result.height, 0);

        // Fill in the tiles from the JSON
        for (const auto& tile : tilesArray) {
            int x = tile["x"].get<int>();
            int y = tile["y"].get<int>();
            int tileId = std::stoi(tile["id"].get<std::string>());

            // Convert pixel position to grid position
            int gridX = (x - minX) / result.tileSize;
            int gridY = (y - minY) / result.tileSize;

            // Store in the grid
            result.tiles[gridX + gridY * result.width] = tileId;
        }

        return result;
    }
};
