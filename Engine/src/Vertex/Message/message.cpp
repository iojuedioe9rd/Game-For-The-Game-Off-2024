#include <vxpch.h>
#include "message.h"
#include "messageBus.h"

namespace Vertex
{
	void Message::send(std::string code, void* context, void* sender, MessagePriority priority)
	{
		MessageBus::post(new Message(code, sender, context, priority));
	}

	void Message::subscribe(std::string code, IMessageHandler* handler)
	{
		MessageBus::addSubscription(code, handler);
	}

	void Message::unsubscribe(std::string code, IMessageHandler* handler)
	{
		MessageBus::removeSubscription(code, handler);
	}
}
