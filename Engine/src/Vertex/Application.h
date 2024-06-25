#pragma once
#include "Vertex/Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "ImGui/ImGuiLayer.h"

namespace Vertex
{

	class VERTEX_API Application
	{
	public:
		Application();
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Run();

		Window& GetWindow() { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		static Application& Get() { return *app; }

		void OnEvent(Event& e);
	private:
		ImGuiLayer* m_ImGuiLayer;
		static Application* app;
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		void Update();
		LayerStack* m_LayerStack = 0;
	};
	extern Vertex::Application* CreateApp();
	

}
