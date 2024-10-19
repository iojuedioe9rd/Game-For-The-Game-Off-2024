#pragma once
#include "Vertex/Core/Base.h"
#include "Window.h"
#include "LayerStack.h"
#include "Vertex/ImGui/ImGuiLayer.h"
#include "Vertex/ImGui/ImGuiWindowStack.h"
#include "Vertex/ImGui/ImGuiWindows/ConsoleWindow.h"
#include "Vertex/CommandBuffer/CommandBufferPool.h"
#include "Vertex/Renderer/Shader.h"
#include "Vertex/Renderer/Buffer.h"
#include "Vertex/Renderer/VertexArray.h"
#include "Vertex/Renderer/OrthographicCamera.h"

namespace Vertex
{

	class VERTEX_API Application
	{
	public:
		Application(const std::string& name = "Vertex App", uint32_t width = 1600, uint32_t height = 900);
		virtual ~Application();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void Run();

		Window& GetWindow() { return *m_Window; }

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
		static Application& Get() { return *app; }
		static glm::vec2 GetWindowSize() { return glm::vec2(app->m_Window->GetWidth(), app->m_Window->GetHeight()); }

		CommandBuffer* GetCommandBuffer() { return m_CommandBufferPool->get(); }
		
		void ReownCommandBuffer(CommandBuffer* commandBuffer) { m_CommandBufferPool->reown(commandBuffer); }
		
		void Close();

		void OnEvent(Event* e);
	private:
		ImGuiLayer* m_ImGuiLayer;
		ImGuiWindows::ConsoleWindow* m_ConsoleWindow;
		static Application* app;
		Ref<Window> m_Window;
		bool m_Minimized = false;
		bool m_Running = true;
		void Update();
		CommandBufferPool* m_CommandBufferPool;
		LayerStack* m_LayerStack = 0;
		ImGuiWindows::ImGuiWindowStack* m_ImGuiWindowStack = 0;

		

		

		OrthographicCamera m_Camera;
	};
	extern Vertex::Application* CreateApp();
	

}
