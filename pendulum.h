#pragma once
#ifndef PENDULUM_H
#define PENDULUM_H

#include "model.h"
#include <string>

namespace Models {
    class ObjModel;

    class Pendulum : public Model {
    public:
        Pendulum(const std::string& filename);
        virtual ~Pendulum();
        virtual void drawSolid(bool smooth = true);
    };

    extern Pendulum pendulum;
}

#endif
