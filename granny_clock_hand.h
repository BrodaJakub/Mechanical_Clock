#pragma once
#ifndef GRANNYCLOCKHAND_H
#define GRANNYCLOCKHAND_H

#include "model.h"
#include <string>

namespace Models {
    class ObjModel;

    class GrannyClockHand : public Model {
    public:
        GrannyClockHand(const std::string& filename);
        virtual ~GrannyClockHand();
        virtual void drawSolid(bool smooth = true);
    };

    extern GrannyClockHand grannyClockHand;
}

#endif
