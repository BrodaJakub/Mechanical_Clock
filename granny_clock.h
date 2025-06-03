#pragma once
#ifndef GRANNYCLOCK_H
#define GRANNYCLOCK_H

#include "model.h"
#include <string>

namespace Models {
    class ObjModel;

    class GrannyClock : public Model {
    public:
        GrannyClock(const std::string& filename);
        virtual ~GrannyClock();
        virtual void drawSolid(bool smooth = true);
    };

    extern GrannyClock grannyClock;
}

#endif
