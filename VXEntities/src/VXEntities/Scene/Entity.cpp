#include "Entity.h"
#include "../Scripting/ScriptEngine.h"

namespace Vertex {

	
	bool Entity::IsNULL()
	{
		if (this == nullptr)
		{
			return true;
		}
		// 0xFFFFFFFFFFFFFFFF
		if (this == (Entity*)0xdddddddddddddddd)
		{
			return true;
		}
		if (this == (Entity*)0xFFFFFFFFFFFFFFFF)
		{
			return true;
		}
		bool hasENT = true;
		Scene* scene = ScriptEngine::GetSceneContext();

		if (scene == nullptr) { return false; }

		for (Entity* ent : *scene)
		{
			if (this == ent) hasENT = false;
		}

		return hasENT;
	}


}