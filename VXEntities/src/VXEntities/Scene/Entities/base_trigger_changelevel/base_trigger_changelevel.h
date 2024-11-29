#pragma once
#include "../../Entity.h"

namespace Vertex
{
	class ENTBaseTriggerChangeLevel : public Entity
	{
	public:

		ENTBaseTriggerChangeLevel() {}
		ENTBaseTriggerChangeLevel(std::string name, Scene* scene) : Entity(name, scene)
		{

		}

		void Trigger();

		std::string filename;

	private:

	};

	
}