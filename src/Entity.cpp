#include "Entity.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <filesystem>

//mainly derived from https://www.sfml-dev.org/tutorials/3.0/graphics/vertex-array/#example-tile-map
bool Entity::loadTexture(const std::filesystem::path& texturePath, sf::Vector2u graphicTileSize, int tiles) {
    if(!texture.loadFromFile(texturePath))
        return false;

    //assumes tiles are animation tiles of one row graphicTileSize height and tiles width
    int height = 1;
    int width = height * tiles;
    int doubleTriVertex = 6;

    //width * height * 2x triangle vertex
    vertexArray.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertexArray.resize(width * height * doubleTriVertex);

    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            const int tileNumber = i; 
            const int tu = tileNumber % (texture.getSize().x / graphicTileSize.x);
            const int tv = tileNumber / (texture.getSize().x / graphicTileSize.x);

            sf::Vertex* triangles = &vertexArray[(i + j * width) * doubleTriVertex];

            triangles[0].position = sf::Vector2f(i * graphicTileSize.x, j * graphicTileSize.y);
            triangles[1].position = sf::Vector2f((i + 1) * graphicTileSize.x, j * graphicTileSize.y);
            triangles[2].position = sf::Vector2f(i * graphicTileSize.x, (j + 1) * graphicTileSize.y);
            triangles[3].position = sf::Vector2f(i * graphicTileSize.x, (j + 1) * graphicTileSize.y);
            triangles[4].position = sf::Vector2f((i + 1) * graphicTileSize.x, j * graphicTileSize.y);
            triangles[5].position = sf::Vector2f((i + 1) * graphicTileSize.x, (j + 1) * graphicTileSize.y);

            triangles[0].texCoords = sf::Vector2f(tu * graphicTileSize.x, tv * graphicTileSize.y);
            triangles[1].texCoords = sf::Vector2f((tu + 1) * graphicTileSize.x, tv * graphicTileSize.y);
            triangles[2].texCoords = sf::Vector2f(tu * graphicTileSize.x, (tv + 1) * graphicTileSize.y);
            triangles[3].texCoords = sf::Vector2f(tu * graphicTileSize.x, (tv + 1) * graphicTileSize.y);
            triangles[4].texCoords = sf::Vector2f((tu + 1) * graphicTileSize.x, tv * graphicTileSize.y);
            triangles[5].texCoords = sf::Vector2f((tu + 1) * graphicTileSize.x, (tv + 1) * graphicTileSize.y);
        }
    }

    return true;
}

void Entity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = &texture;
    target.draw(vertexArray, states);
};


