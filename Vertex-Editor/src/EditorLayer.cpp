#include "EditorLayer.h"
#include "Vertex/Renderer/Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Panels/SceneHierarchyPanel.h"
#include "Vertex/Core/Input.h"
#include "Vertex/Utils/Utils.h"
#include "Vertex/ImGui/ImGuizmoLink.h"
#include "Vertex/Math/Math.h"
#include <VXEntities/Scene/EditorCamera.h>
#include "resource.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <VXEntities/Scripting/ScriptEngine.h>
#include "Vertex/Message/message.h"
#include "Vertex/Message/messageBus.h"
#include "VXEntities/Messages/LoadMap.h"
#include "Vertex/Renderer/VertexArray.h"
#include "Vertex/Renderer/Buffer.h"

//Discord SDK Stuff.
#include "DiscordRPC/discord_rpc.h"
#include "DiscordRPC/discord_register.h"

static bool gInit, gRPC = true;

std::string wstringToString(const std::wstring& wstr) {
	std::string str(wstr.begin(), wstr.end());
	return str;
}

std::wstring LoadResourceString(UINT resourceID) {
	wchar_t buffer[256];  // Buffer to hold the loaded string
	if (LoadStringW(GetModuleHandle(NULL), resourceID, buffer, sizeof(buffer) / sizeof(wchar_t))) {
		return buffer;
	}
	return L"";  // Return an empty string if loading fails
}

void SetupDiscord()
{
	if (gRPC)
	{
		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		std::string myline;
		Discord_Initialize(wstringToString(LoadResourceString(IDS_STRING106)).c_str(), &handlers, 1, "0");
	}
	else
	{
		Discord_Shutdown();
	}
}


static void UpdateDiscord()
{
	static int64_t StartTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

	if (gRPC)
	{
		DiscordRichPresence discordPresence;
		memset(&discordPresence, 0, sizeof(discordPresence));
		discordPresence.state = "I am the RPC Title.";
		discordPresence.details = "I am just a simple bio.";
		discordPresence.startTimestamp = StartTime;
		discordPresence.endTimestamp = NULL;
		discordPresence.instance = 1;

		Discord_UpdatePresence(&discordPresence);
	}
	else
	{
		Discord_ClearPresence();
	}
}


namespace Vertex {

	float quadVertices[] = {
		// Positions      // Texture Coords
	   -1.0f, -1.0f,     0.0f, 0.0f,  // Bottom-left
		1.0f, -1.0f,     1.0f, 0.0f,  // Bottom-right
		1.0f,  1.0f,     1.0f, 1.0f,  // Top-right
	   -1.0f,  1.0f,     0.0f, 1.0f   // Top-left
	};

	uint32_t quadIndices[] = {
		0, 1, 2,  // First triangle
		2, 3, 0   // Second triangle
	};

	std::shared_ptr<Vertex::VertexArray> Quad;

	std::shared_ptr<Vertex::VertexArray> SetupFullScreenQuad()
	{
		auto quadVAO = std::shared_ptr<Vertex::VertexArray>(Vertex::VertexArray::Create());

		// Create Vertex Buffer
		auto quadVBO = std::shared_ptr<Vertex::VertexBuffer>(Vertex::VertexBuffer::Create(quadVertices, sizeof(quadVertices)));

		// Define the buffer layout
		Vertex::BufferLayout layout = {
			{ Vertex::ShaderDataType::Float2, "a_Position" },
			{ Vertex::ShaderDataType::Float2, "a_TexCoord" }
		};
		quadVBO->SetLayout(layout);

		// Add Vertex Buffer to Vertex Array
		quadVAO->AddVertexBuffer(quadVBO);

		// Create Index Buffer
		auto quadIBO = std::shared_ptr<Vertex::IndexBuffer>(Vertex::IndexBuffer::Create(quadIndices, sizeof(quadIndices) / sizeof(uint32_t)));

		// Set Index Buffer for Vertex Array
		quadVAO->SetIndexBuffer(quadIBO);

		return quadVAO;
	}

	extern const std::filesystem::path g_AssetPath;

	EditorLayer::EditorLayer()
		: Layer("EditorLayer"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
	{
		Message::subscribe("LOAD_MAP", this);
		Message* message = new Message("LOAD_MAP", this, new std::string("Map"), MessagePriority::NORMAL);
		MessageBus::post(message);
		SetupDiscord();
		gInit = false;
	}

	Ref<Texture2D> CheckerboardTexture;

	void EditorLayer::OnAttach()
	{
		VX_PROFILE_FUNCTION();

		Quad = SetupFullScreenQuad();
		m_FlatColorShader = Shader::Create("assets/shaders/FlatColor.glsl");

		CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");
		this->m_CheckerboardTexture = CheckerboardTexture;
		m_IconPlay = Texture2D::CreateWin(IDB_PNG4, "PNG");
		m_IconStop = Texture2D::CreateWin(IDB_PNG5, "PNG");

		FramebufferSpecification fbSpec;
		fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.Width = 1600;
		fbSpec.Height = 900;

		FramebufferSpecification fbSpec2;
		fbSpec2.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec2.Width = 1600;
		fbSpec2.Height = 900;


		m_CameraController.GetCamera().SetWindowSize(1280, 720);
		m_CameraController.GetCamera().SetPosition(glm::vec3(0,0,0));



		m_Framebuffer = Framebuffer::Create(fbSpec);
		m_Framebuffer2 = Framebuffer::Create(fbSpec2);

		m_ActiveScene = VXEntities_MakeOrGetScene("ActiveScene");

		auto commandLineArgs = Application::Get().GetCommandLineArgs();
		if (commandLineArgs.Count > 1)
		{
			auto sceneFilePath = commandLineArgs[1];
			SceneSerializer serializer(&m_ActiveScene);
			OpenScene(sceneFilePath);
			m_EditorScene = m_ActiveScene;
		}

		m_EditorCamera = EditorCamera(30.0f, 1.778f, 0.1f, 1000.0f);

		// Entity

		auto& square = m_ActiveScene->CreateEntity<ENTPropStaticSprite>("Green Square");
		
		m_ActiveScene->CreateEntity<ENTEnvStaticTilemap>("Tilemap").AddTile(glm::i32vec2(1, 5), nullptr, m_SquareColor);
		
		m_CameraEntity = &m_ActiveScene->CreateEntity<ENTPointCamera2D>("Camera Entity");
		
		m_CameraEntity->isPrimary = true;

		m_SecondCamera = &m_ActiveScene->CreateEntity<ENTPointCamera2D>("Clip-Space Entity");
		
		m_SecondCamera->isPrimary = false;

		

		
		square.colour = glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
		square.SetIsVisible(true);

		m_SquareEntity = square;
		m_SquareEntity.colour = glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);

		
		GImGui = (ImGuiContext*)ImGuiLink::GetContext();

		// Load models

		float vertices[] =
		{
			//     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
			-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side
			-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
			0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,      0.0f, -1.0f, 0.0f, // Bottom side
			0.5f, 0.0f,  0.5f,      0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      0.0f, -1.0f, 0.0f, // Bottom side

			-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,      -0.8f, 0.5f,  0.0f, // Left Side
			-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,      -0.8f, 0.5f,  0.0f, // Left Side
			0.0f, 0.8f,  0.0f,      0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,      -0.8f, 0.5f,  0.0f, // Left Side

			-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,     5.0f, 0.0f,       0.0f, 0.5f, -0.8f, // Non-facing side
			0.5f, 0.0f, -0.5f,      0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,       0.0f, 0.5f, -0.8f, // Non-facing side
			0.0f, 0.8f,  0.0f,      0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,       0.0f, 0.5f, -0.8f, // Non-facing side

			0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	     0.0f, 0.0f,       0.8f, 0.5f,  0.0f, // Right side
			0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	     5.0f, 0.0f,       0.8f, 0.5f,  0.0f, // Right side
			0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	     2.5f, 5.0f,       0.8f, 0.5f,  0.0f, // Right side

			0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	     5.0f, 0.0f,       0.0f, 0.5f,  0.8f, // Facing side
			-0.5f, 0.0f,  0.5f,    0.83f, 0.70f, 0.44f,      0.0f, 0.0f,       0.0f, 0.5f,  0.8f, // Facing side
			0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	     2.5f, 5.0f,       0.0f, 0.5f,  0.8f  // Facing side
		};



		

		Ref<VertexBuffer> VBO = Ref<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
		VBO->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Colour" },
			{ ShaderDataType::Float2, "a_TexCoord" },
			{ ShaderDataType::Float3, "a_Normal" }
		});

		unsigned int indices[] =
		{
			0, 1, 2, // Bottom side
			0, 2, 3, // Bottom side
			4, 6, 5, // Left side
			7, 9, 8, // Non-facing side
			10, 12, 11, // Right side
			13, 15, 14 // Facing side
		};


		int indicesSize = sizeof(indices) / sizeof(unsigned int);
		Ref<IndexBuffer> indexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices, indicesSize));


		m_3DShader = Shader::Create("assets/shaders/3DModel.glsl");
		m_3DVA = Ref<VertexArray>(VertexArray::Create());
		m_3DVA->AddVertexBuffer(VBO);
		m_3DVA->SetIndexBuffer(indexBuffer);


		OpenScene();
	}

	float t = 0.0f;

	
	void DrawFullScreenQuad(Ref<Shader> m_FlatColorShader, Framebuffer* m_Framebuffer2)
	{
		// Step 1: Define the vertex data for a full-screen quad
		float vertices[] = {
			// Positions         // Texture coordinates
			-1.0f,  1.0f, 0.0f, 1.0f, 1.0f, // Top left
			-1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Bottom left
			 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Bottom right
			 1.0f,  1.0f, 0.0f, 0.0f, 1.0f  // Top right
		};

		unsigned int indices[] = {
			0, 1, 2, 0, 2, 3  // Two triangles to make a quad
		};

		// Step 2: Create the vertex buffer and index buffer
		Ref<VertexBuffer> vertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(vertices, sizeof(vertices)));
		Ref<IndexBuffer> indexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices, 6));

		// Step 3: Create a VertexArray to hold the buffers
		Ref<VertexArray> vertexArray = Ref<VertexArray>(VertexArray::Create());
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_UV" },
			});
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);

		// Step 4: Upload the texture uniform and bind the shader
		m_FlatColorShader->UploadUniformInt("u_Texture", 0); // Assuming 'u_Texture' is the uniform name in the shader
		m_FlatColorShader->UploadUniformFloat2("u_Res", Application::GetWindowSize());
		m_FlatColorShader->UploadUniformFloat("u_Time", t);
		m_FlatColorShader->Bind();

		// Bind the framebuffer texture to texture unit 0
		m_Framebuffer2->BindAsTex(0);

		// Step 5: Bind the vertex array and issue the draw call
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray, indexBuffer->GetCount());
		vertexArray->Unbind();
	}


	void EditorLayer::OnDetach()
	{
		VX_PROFILE_FUNCTION();
	}
	float rotation = 0.0f;
	void EditorLayer::OnUpdate(Timestep ts)
	{
		UpdateDiscord();

		if (m_EditorScene == nullptr)
		{
			// Handle initialization or fallback logic
		}

		t += ts;

		VX_PROFILE_FUNCTION();

		// Render
		Renderer2D::ResetStats();
		m_Framebuffer2->Bind();
		RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		RenderCommand::Clear();
		m_Framebuffer2->ClearAttachment(1, -1);

		if (m_SceneState == SceneState::Edit)
		{
			if (m_ViewportFocused)
				m_CameraController.OnUpdate(ts);
			m_EditorCamera.OnUpdate(ts);

			Renderer2D::BeginScene(m_EditorCamera.GetViewProjection(), m_Colours[0], m_Colours[1], m_Colours[2]);
			m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
			Renderer2D::EndScene();
		}
		else if (m_SceneState == SceneState::Play)
		{
			Ref<Camera> cam;
			glm::mat4 p_cam;
			if (m_ActiveScene->GetACameraInScene(&cam, true, &p_cam))
			{
				Renderer2D::BeginScene(cam->GetProjection(), p_cam, m_Colours[0], m_Colours[1], m_Colours[2]);
				m_ActiveScene->OnUpdateRuntime(ts);
				Renderer2D::EndScene();

				
				
				


				
			}
		}

		// Handle mouse position and entity selection
		auto mx = ImGuiLink::GetMousePos().x;
		auto my = ImGuiLink::GetMousePos().y;
		mx -= m_ViewportBounds[0].x;
		my -= m_ViewportBounds[0].y;
		glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		my = viewportSize.y - my;  // Flip the Y-axis to match viewport coordinate system
		int mouseX = static_cast<int>(mx);
		int mouseY = static_cast<int>(my);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) && mouseY < static_cast<int>(viewportSize.y))
		{
			int pixelData = m_Framebuffer2->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = nullptr;

			for (Entity* ent : *m_ActiveScene)
			{
				int id = 0;
				for (char c : ent->GetID())
				{
					id += c;
				}

				if (id == pixelData)
				{
					VX_INFO("m_HoveredEntity is {0}", ent->name().c_str());
					m_HoveredEntity = ent;
					break;
				}
			}
		}
		Renderer2D::EndScene();

		if (m_SceneState == SceneState::Play)
		{
			Ref<Camera> cam;
			glm::mat4 p_cam;
			if (m_ActiveScene->GetACameraInScene(&cam, true, &p_cam))
			{
				m_3DShader->Bind();

				rotation += 30 * ts;

				glm::mat4 model = Math::ComposeTransform({ 0, 0, 1 }, { 1, 1, 1 }, { 0, rotation, 0 });
				glm::mat4 view = glm::mat4(1.0f);
				glm::mat4 proj = glm::mat4(1.0f);
				proj = cam->GetProjection();

				m_CheckerboardTexture->Bind();
				


				m_3DShader->UploadUniformInt("u_Tex0", 0); // Assuming 'u_Texture' is the uniform name in the shader
				m_3DShader->UploadUniformMat4("u_Model", model);
				m_3DShader->UploadUniformMat4("u_View", proj * glm::inverse(p_cam));
				m_3DShader->UploadUniformMat4("u_Proj", glm::mat4(1.0f));

				

				RenderCommand::DrawIndexed(m_3DVA, m_3DVA->GetIndexBuffer()->GetCount());
				m_3DShader->Unbind();
			}

		}

		Renderer::De();
		
		
		

		// Post-process or apply full-screen effects after rendering
		m_Framebuffer2->Unbind();

		m_Framebuffer->Bind();
		
		DrawFullScreenQuad(m_FlatColorShader, m_Framebuffer2);
		RenderCommand::Init();

		m_Framebuffer->Unbind();

		
	}


	void EditorLayer::OnImGuiRender()
	{
		VX_PROFILE_FUNCTION();

		// Note: Switch this to true to enable dockspace
		static bool dockingEnabled = 1;

		ImGuiLink::Docking(dockingEnabled, [this] { DockSpaceCallback(); });

		if (!dockingEnabled)
		{
			
			ImGuiLink::Begin("Settings");

			auto stats = Renderer2D::GetStats();
			ImGuiLink::Text("Renderer2D Stats:");
			ImGuiLink::Text("Draw Calls: %d", stats.DrawCalls);
			ImGuiLink::Text("Quads: %d", stats.QuadCount);
			ImGuiLink::Text("Vertices: %d", stats.GetTotalVertexCount());
			ImGuiLink::Text("Indices: %d", stats.GetTotalIndexCount());

			ImGuiLink::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));

			uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
			ImGuiLink::Image((void*)textureID, glm::vec2(200, 200), glm::vec2{0, 1}, glm::vec2{1, 0});

			

			ImGuiLink::End();
			
		}
		
	}

	void EditorLayer::OnEvent(Event& e)
	{
		m_CameraController.OnEvent(e);
		m_EditorCamera.OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VX_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
		
	}

	void EditorLayer::onMessage(Message* message)
	{
		if (message->code == "LOAD_MAP")
		{
			LoadMapMessage* map = (LoadMapMessage*)message;
			VX_INFO(map->GetMapName().c_str());
		}
	}

	void EditorLayer::DockSpaceCallback()
	{
		if (ImGuiLink::BeginMenuBar())
		{
			if (ImGuiLink::BeginMenu("File"))
			{
				if (ImGuiLink::MenuItem("Serialize"))
				{
					SaveSceneAs();
				}

				if (ImGuiLink::MenuItem("Deserialize"))
				{
					OpenScene();
				}
				ImGuiLink::EndMenu();
			}
			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload assembly", "Ctrl+R"))
					ScriptEngine::ReloadAssembly();

				ImGui::EndMenu();
			}
			ImGuiLink::EndMenuBar();
		}

		ImGui::Begin("Settings");

		auto stats = Renderer2D::GetStats();
		ImGui::Text("Renderer2D Stats:");
		ImGui::Text("Draw Calls: %d", stats.DrawCalls);
		ImGui::Text("Quads: %d", stats.QuadCount);
		ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
		ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

		ImGui::Text("FPS: %d", static_cast<int>(std::round(Time::GetFPS())));

		ImGui::Text("Colours");
		for (int i = 0; i < 3; i++)
		{
			ImGui::ColorEdit4((std::string("Colour ") + std::to_string(i + 1)).c_str(), glm::value_ptr(m_Colours[i]));
		}

		ImGui::Text("Ticks %d", Time::GetTicks());

		if (true)
		{
			ImGuiLink::Separator();
			
			ImGuiLink::Text("%s", m_SquareEntity.name());

			m_SquareEntity.colour = m_SquareColor;
			ImGuiLink::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
			ImGuiLink::Separator();

		}
		ImGuiLink::ColorEdit3("Camera Transform", glm::value_ptr(m_CameraEntity->pos));

		if (ImGuiLink::Checkbox("Camera A", &m_PrimaryCamera))
		{
			m_CameraEntity->isPrimary = m_PrimaryCamera;
			m_SecondCamera->isPrimary = !m_PrimaryCamera;

		}

		ImGui::End();

		ImGuiLink::PushStyleVar(ImGuiLink::ImGuiStyleVar_WindowPadding, glm::vec2{ 0, 0 });
		ImGuiLink::Begin("Viewport");
		auto viewportOffset = ImGuiLink::GetCursorPos(); // Includes tab bar

		m_ViewportFocused = ImGuiLink::IsWindowFocused();
		m_ViewportHovered = ImGuiLink::IsWindowHovered();

		Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);


		glm::vec2 viewportPanelSize = ImGuiLink::GetContentRegionAvail();
		m_ActiveScene->OnViewportResize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
		if (m_ViewportSize != viewportPanelSize)
		{
			m_Framebuffer->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_Framebuffer2->Resize((uint32_t)viewportPanelSize.x, (uint32_t)viewportPanelSize.y);
			m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

			m_CameraController.OnResize(viewportPanelSize.x, viewportPanelSize.y);
			m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		}

		uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
		ImGuiLink::Image((void*)textureID, glm::vec2{ viewportPanelSize.x, viewportPanelSize.y }, glm::vec2{ 1, 0 }, glm::vec2{ 0, 1 });

		auto windowSize = ImGuiLink::GetWindowSize();
		auto minBound = ImGuiLink::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		glm::vec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };

		if (ImGuiLink::BeginDragDropTarget())
		{
			if (const ImGuiLink::ImGuiPayload* payload = ImGuiLink::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				const wchar_t* path = (const wchar_t*)payload->Data;
				OpenScene(std::filesystem::path(g_AssetPath) / path);
			}
			ImGuiLink::EndDragDropTarget();
		}

		Entity* selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
		if (selectedEntity != nullptr && m_GizmoType != -1)
		{
			ImGuizmoLink::SetOrthographic(false);
			ImGuizmoLink::SetDrawlist();

			ImGuizmoLink::SetRect(glm::vec4(0,0, ImGuiLink::GetWindowSize()));

			const auto& camera = cam->camera;
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			glm::mat4 transform = Vertex::Math::ComposeTransform(selectedEntity->pos, selectedEntity->size, selectedEntity->rotation);

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f;

			if (m_GizmoType == ImGuizmoLink::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmoLink::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmoLink::OPERATION)m_GizmoType, ImGuizmoLink::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (true)
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);
				glm::vec3 deltaRotation = rotation - selectedEntity->rotation;
				selectedEntity->pos = translation;
				selectedEntity->rotation += deltaRotation;
				selectedEntity->size = scale;
			}
		}

		ImGuiLink::End();
		ImGuiLink::PopStyleVar();

		m_SceneHierarchyPanel.OnImGuiRender();
		m_ContentBrowserPanel.OnImGuiRender();

		UI_Toolbar();
		
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		// Shortcuts
		if (e.GetRepeatCount() > 0)
			return false;

		bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
		bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
		switch (e.GetKeyCode())
		{
		case KeyCode::N:
		{
			if (control)
				NewScene();

			break;
		}
		case Key::O:
		{
			if (control)
				OpenScene();

			break;
		}
		case Key::S:
		{
			if (control && shift)
				SaveSceneAs();

			break;
		}

		// Gizmos
		case Key::Q:
			m_GizmoType = -1;
			break;
		case Key::W:
			m_GizmoType = ImGuizmoLink::OPERATION::TRANSLATE;
			break;
		case Key::E:
			m_GizmoType = ImGuizmoLink::OPERATION::ROTATE;
			break;
		case Key::R:
			m_GizmoType = ImGuizmoLink::OPERATION::SCALE;
			break;
		}
	}

	void EditorLayer::NewScene()
	{
		
		VXEntities_RemoveScene(m_ActiveScene);
		m_ActiveScene = VXEntities_MakeOrGetScene("ActiveScene");
		m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_EditorScene = m_ActiveScene;
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialogs::OpenFile("Vertex Scene (*.vertex)\0*.vertex\0");
		if (!filepath.empty())
		{
			OpenScene(filepath);
		}
	}

	void EditorLayer::SaveSceneAs()
	{
		std::string filepath = FileDialogs::SaveFile("Vertex Scene (*.vertex)\0*.vertex\0");
		if (!filepath.empty())
		{
			SceneSerializer serializer(&m_ActiveScene);
			serializer.Serialize(filepath);
		}
	}

	void EditorLayer::OpenScene(const std::filesystem::path& path)
	{
		if (m_SceneState != SceneState::Edit)
			OnSceneStop();

		NewScene();
		SceneSerializer serializer(&m_EditorScene);
		if(serializer.Deserialize(path.string()));
		{
			m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
			m_SceneHierarchyPanel.SetContext(m_EditorScene);

			m_ActiveScene = m_EditorScene;
			m_EditorScenePath = path;
		}
	}

	void EditorLayer::SaveScene()
	{
		if (!m_EditorScenePath.empty())
			SerializeScene(m_ActiveScene, m_EditorScenePath);
		else
			SaveSceneAs();
	}

	void EditorLayer::SerializeScene(Scene* scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(&scene);
		serializer.Serialize(path.string());
	}

	void EditorLayer::OnScenePlay()
	{
		m_SceneState = SceneState::Play;

		m_ActiveScene = Scene::Copy(m_EditorScene, std::string("Runtime"));
		m_ActiveScene->OnRuntimeStart();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::OnSceneStop()
	{
		m_SceneState = SceneState::Edit;

		m_ActiveScene->OnRuntimeStop();
		VXEntities_RemoveScene(m_ActiveScene);
		m_ActiveScene = m_EditorScene;

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::UI_Toolbar()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Ref<Texture2D> icon = m_SceneState == SceneState::Edit ? m_IconPlay : m_IconStop;

		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x * 0.5f) - (size * 0.5f));

		if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_SceneState == SceneState::Edit)
				OnScenePlay();
			else if (m_SceneState == SceneState::Play)
				OnSceneStop();
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();

	}

}

