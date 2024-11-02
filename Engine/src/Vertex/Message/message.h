#pragma once
#include "IMessageHandler.h"
#include <string>

namespace Vertex
{
	enum class VERTEX_API MessagePriority
	{
		NORMAL,
		HIGH
	};

	class VERTEX_API Message {
	public:
		std::string code;

		/* context is data.  I don't know why I named it that */
		void* context;

		void* sender;

		MessagePriority priority = MessagePriority::NORMAL;

		Message(std::string code, void* sender, void* context, MessagePriority priority)
		{
			this->code = code;
			this->sender = sender;
			this->context = context;
			this->priority = priority;
		}

		static void send(std::string code, void* context, void* sender, MessagePriority priority = MessagePriority::NORMAL);
		static void subscribe(std::string code, IMessageHandler* handler);
		static void unsubscribe(std::string code, IMessageHandler* handler);

	};

}