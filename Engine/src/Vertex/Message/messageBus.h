#pragma once
#include "Vertex/Core/Base.h"
#include "MessageSubscriptionNode.h"
#include "IMessageHandler.h"
#include <map>
#include <vector>

namespace Vertex
{
	class VERTEX_API MessageBus
	{
	public:
		static void addSubscription(std::string code, IMessageHandler* handler);
		static void removeSubscription(std::string code, IMessageHandler* handler);

		static void post(Message* message);

		static void update();

	private:
		MessageBus() {}
		~MessageBus() {}

		static std::map<std::string, std::vector<IMessageHandler*>> m_Subscriptions;

		static std::vector<MessageSubscriptionNode> m_NormalMessageQueue;
	};

	
}