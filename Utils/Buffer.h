//
// Created by Kevin on 18/08/2020.
//

#pragma once

#include <vector>
#include <memory>

template<class T>
class Buffer {
public:
    Buffer(int width, int height) : width(width), height(height){

        data = std::vector<std::vector<T>>(height);
        for (int i = 0; i < height; ++i) {
            data[i] = std::vector<T>(width);
        }
    }

    T& operator()(int row, int column) {
        return at(row, column);
    }

    T& at(int row, int column) {
        return data[row][column];
    }

    T at(int row, int column) const {
        return data[row][column];
    }
    T operator()(int row, int column) const {
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
    int width;
    int height;


};
