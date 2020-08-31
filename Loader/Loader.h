//
// Created by kevin on 17/08/20.
//

#pragma once

#include <string>

template<class T>
class Loader {
public:


    virtual T load(std::string path) const = 0;

};