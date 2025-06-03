#pragma once
#ifndef GRANNYCLOCKFACE_H
#define GRANNYCLOCKFACE_H

#include "model.h"
#include <string>

namespace Models {
    class ObjModel;

    class GrannyClockFace : public Model {
    public:
        GrannyClockFace(const std::string& filename);
        virtual ~GrannyClockFace();
        virtual void drawSolid(bool smooth = true);
    };

    extern GrannyClockFace grannyClockFace;
}

#endif
