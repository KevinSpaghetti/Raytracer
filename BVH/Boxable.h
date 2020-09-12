//
// Created by kevin on 04/09/20.
//

#pragma once

//Indicates the ability of a class that represents 3d object to be enclosed
//in a bounding box
template<typename BoxType>
class Boxable {
public:
    virtual BoxType getSurroundingBox() const = 0;

};