#include "trigger_changelevel_circle.h"
#include <glm/gtx/norm.hpp>

namespace Vertex
{
	void ENTTriggerChangeLevelCircle::Update(Timestep& ts)
	{
		Entity* ent = m_Scene->FindEntityByName(TargetName);

		if (ent && glm::distance(pos, ent->pos) <= CircleSize)
		{
			Trigger();
		}
	}
}