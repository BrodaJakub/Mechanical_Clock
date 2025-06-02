#ifndef OBJMODEL_H
#define OBJMODEL_H

#include "model.h"
#include <string>

namespace Models {
	class ObjModel : public Model {
	public:
		ObjModel(const std::string& filename);
		virtual ~ObjModel();
		virtual void drawSolid(bool smooth = true);
	};

	extern ObjModel gearwheel;
}

#endif
