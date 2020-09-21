//
// Created by kevin on 20/08/20.
//

#pragma once

#include <iostream>
#include <cmath>
#include "Buffer.h"

template<typename T>
class Tile {
public:
    Tile(Buffer<T>& buffer, glm::vec2 start, glm::vec2 dimensions) : buffer(buffer), start(start), dimensions(dimensions){}

    //Split the Rect in n*n smaller rects
    //0 split: The full tile
    //1 split: 2 Tiles
    //2 split: 4 Tiles
    static std::vector<Tile> split(Buffer<T>& r, int n){
        int ntiles = pow(n, 2);

        std::cout << "Splitting in: " << ntiles << " tiles \n";
        std::cout << "Splitting Buffer: (" << 0 << ", " << 0 << ") : (" << r.getWidth() << ", " << r.getHeight() << ")\n";

        std::vector<Tile> tiles;
        if (n == 0 || n == 1){
            tiles.emplace_back(Tile<T>(r, {0, 0}, {r.getWidth(), r.getHeight()}));
            return tiles;
        }

        int width_splits = n;
        int height_splits = n;

        std::cout << "Width splits: " << width_splits << "\n";
        std::cout << "Height splits: " << height_splits << "\n";

        int tile_width = r.getWidth() / width_splits;
        int tile_height = r.getHeight() / height_splits;

        std::cout << "Tile width: " << tile_width << "\n";
        std::cout << "Tile height: " << tile_height << "\n";

        tiles.reserve(ntiles);
        for (int i = 0; i < r.getWidth(); i += tile_width) {
            for (int j = 0; j < r.getHeight(); j += tile_height) {
                Tile<T> t(r, {i, j}, {tile_width, tile_height});
                tiles.push_back(t);
            }
        }

        return tiles;
    }

    T& operator()(int row, int column){
        return buffer(start.y + row, start.x + column);
    }

    glm::vec2 getStart(){
        return start;
    }
    glm::vec2 getDimensions(){
        return dimensions;
    }


private:
    Buffer<T>& buffer;
    glm::vec2 start;
    glm::vec2 dimensions;
};