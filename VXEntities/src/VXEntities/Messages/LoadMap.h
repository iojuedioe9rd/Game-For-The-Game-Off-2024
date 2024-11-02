#pragma once
#include <Vertex/Message/message.h>

namespace Vertex
{
	class LoadMapMessage : Message
	{
	public:

		std::string GetMapName() const
		{
			return *(std::string*)context;
		}


	private:

	};

	
}