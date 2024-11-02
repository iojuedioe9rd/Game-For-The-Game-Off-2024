#pragma once
#include <Vertex/Core/Base.h>



namespace Vertex
{
	class Message;

	class VERTEX_API IMessageHandler
	{
	public:
		IMessageHandler() {}
		~IMessageHandler() {}

		virtual void onMessage(Message* message) = 0;

	private:

	};

	
}