#pragma once
#include <Vertex/Core/Base.h>
#include "message.h"
#include "IMessageHandler.h"

namespace Vertex
{
	class VERTEX_API MessageSubscriptionNode
	{
	public:
		Message* message;

		IMessageHandler* handler;

		MessageSubscriptionNode(Message* message, IMessageHandler* handler)
		{
			this->message = message;
			this->handler = handler;
		}
	};
}