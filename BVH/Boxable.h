//
// Created by kevin on 04/09/20.
//

#pragma once

#include "BoundingBox.h"

//Indicates the ability of a class that represents 3d object to be enclosed
//in a bounding box
class Boxable {
public:
    virtual shared_ptr<BoundingBox> getSurroundingBox() = 0;

};