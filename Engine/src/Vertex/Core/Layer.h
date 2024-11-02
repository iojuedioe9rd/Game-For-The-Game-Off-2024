#pragma once

#include "Base.h"
#include <string>
#include "Vertex/Events/Event.h"
#include "Vertex/Core/Timestep.h"
#include "Vertex/Message/IMessageHandler.h"

namespace Vertex
{
	class VERTEX_API Layer : public IMessageHandler
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		virtual void onMessage(Message* message) override {}

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};
}