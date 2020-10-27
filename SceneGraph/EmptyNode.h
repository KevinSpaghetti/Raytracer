//
// Created by kevin on 15/10/20.
//

#pragma once

#include "Node.h"

class EmptyNode : public Node {

    Type type() const override {
        return Type::Empty;
    }
};


