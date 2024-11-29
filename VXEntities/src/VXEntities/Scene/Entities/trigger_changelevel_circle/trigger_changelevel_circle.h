#pragma once
#include "../../Entity.h"
#include "../base_trigger_changelevel/base_trigger_changelevel.h"

namespace Vertex
{
	class ENTTriggerChangeLevelCircle : public ENTBaseTriggerChangeLevel
	{
	public:
		float CircleSize = 0.5f;
		std::string TargetName = "Player";

		ENTTriggerChangeLevelCircle() {}
		ENTTriggerChangeLevelCircle(std::string name, Scene* scene) : ENTBaseTriggerChangeLevel(name, scene)
		{

		}

		virtual void Update(Timestep& ts) override;
		virtual void Draw(Timestep& ts)   override {}
		virtual std::string GetEntName()  override
		{
			return "trigger_changelevel_circle";
		}
		virtual void EventH(Event& e) override
		{

		}

	private:

	};




}