#include <vxpch.h>
#include "messageBus.h"

template<typename T>
T& pop_front(std::vector<T>& vec)
{
	T vel = vec.front();
	vec.erase(vec.begin());
	return vel;
}

namespace Vertex
{
	std::map<std::string, std::vector<IMessageHandler*>> MessageBus::m_Subscriptions;

	std::vector<MessageSubscriptionNode> MessageBus::m_NormalMessageQueue;

	void MessageBus::addSubscription(std::string code, IMessageHandler* handler)
	{
		if(!(m_Subscriptions.find(code) != m_Subscriptions.end())) {
			m_Subscriptions[code] = std::vector<IMessageHandler*>();
		}

		int cnt = std::count(m_Subscriptions[code].begin(), m_Subscriptions[code].end(), handler);
		if (cnt > 0)
			return;
		m_Subscriptions[code].push_back(handler);
		

	}

	void MessageBus::removeSubscription(std::string code, IMessageHandler* handler)
	{
		if (!(m_Subscriptions.find(code) != m_Subscriptions.end())) {
			return;
		}

		int cnt = std::count(m_Subscriptions[code].begin(), m_Subscriptions[code].end(), handler);
		if (cnt > 0)
		{
			auto it = std::find(m_Subscriptions[code].begin(), m_Subscriptions[code].end(), handler);
			if (it != m_Subscriptions[code].end())
				m_Subscriptions[code].erase(it);
		}
	}

	void MessageBus::post(Message* message)
	{
		if (!(m_Subscriptions.find(message->code) != m_Subscriptions.end())) {
			return;
		}
		auto& handlers = m_Subscriptions[message->code];

		for (auto h: handlers)
		{
			if (message->priority == MessagePriority::HIGH)
			{
				h->onMessage(message);
			}
			else
			{
				m_NormalMessageQueue.push_back(MessageSubscriptionNode(message, h));
			}
		}
	}

	void MessageBus::update()
	{
		if (m_NormalMessageQueue.size() == 0) return;
		int messageLimit = std::min((int)m_NormalMessageQueue.size(), 10);
		for (int i = 0; i < messageLimit; i++)
		{
			if (m_NormalMessageQueue.front().handler == nullptr) continue;
			auto& node = pop_front(m_NormalMessageQueue);
			node.handler->onMessage(node.message);
		}

	}
}