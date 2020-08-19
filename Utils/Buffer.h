//
// Created by Kevin on 18/08/2020.
//

#pragma once

#include <vector>
#include <memory>

template<class T>
class Buffer {
public:
    Buffer(size_t width, size_t height) : width(width), height(height){
        data.reserve(height);
        for (int i = 0; i < height; ++i) {
            std::vector<T> row(width);
            data.push_back(std::move(row));
        }
    }

    T& operator()(int row, int column){
        return data[row][column];
    }

    size_t getWidth() const {
        return width;
    }

    size_t getHeight() const {
        return height;
    }


private:
    std::vector<std::vector<T>> data;
    //In case we wanted to change the implementation using
    //something that does not remember the dimensions
    size_t width;
    size_t height;


};
