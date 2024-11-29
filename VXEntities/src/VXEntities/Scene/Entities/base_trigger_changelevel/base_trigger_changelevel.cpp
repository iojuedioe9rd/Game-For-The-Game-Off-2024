#include "base_trigger_changelevel.h"
#include "../../../Messages/LoadMap.h"
#include <Vertex/Message/messageBus.h>

namespace Vertex
{
	void ENTBaseTriggerChangeLevel::Trigger()
	{
		std::string* name = new std::string(filename);
		LoadMapMessage* loadMapMessage = new LoadMapMessage("LOAD_MAP", this, name, MessagePriority::HIGH);
		MessageBus::post((Message*)(void*)loadMapMessage);
	}
}