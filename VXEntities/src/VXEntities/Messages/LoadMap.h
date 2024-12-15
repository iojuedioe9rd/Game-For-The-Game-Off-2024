#pragma once
#include <Vertex/Message/message.h>

namespace Vertex
{
	class LoadMapMessage : public Message
	{
	public:

		LoadMapMessage(std::string code, void* sender, void* context, MessagePriority priority) : Message(code, sender, context, priority)
		{
		}

		std::string GetMapName() const
		{
			return *(std::string*)context;
		}


	private:

	};

	
}