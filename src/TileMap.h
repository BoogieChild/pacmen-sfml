#ifndef TILEMAP_H
#define TILEMAP_H

#include <SFML/Graphics.hpp>

#include <vector>
#include <filesystem>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    std::vector<int> tiles;
    unsigned int width;
    unsigned int height;
    unsigned int tileSize;

    void loadFromJSON(const std::filesystem::path& jsonPath);

    bool load(const std::filesystem::path& tileset, sf::Vector2u tileSize, const int* tiles, unsigned int width, unsigned int height);

    int convert2DCoords(int row, int col);

    int convert2DCoords(sf::Vector2i vec);

    int getTileId(int tileX, int tileY); 

    int getTileId(sf::Vector2i vec);

    bool isLegalTile(sf::Vector2i vec); 

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::VertexArray vertices;
    sf::Texture tileset;
};

#endif
