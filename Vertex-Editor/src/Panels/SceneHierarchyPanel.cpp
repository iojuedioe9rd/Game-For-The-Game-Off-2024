#include "SceneHierarchyPanel.h"
#include "Vertex/ImGui/ImGuiLink.h"
#include <glm/gtc/type_ptr.hpp>
#include "VXEntities.h"

#define ImGuiTreeNodeFlags_Selected 1
#define ImGuiTreeNodeFlags_OpenOnArrow 128
#define ImGuiTreeNodeFlags_DefaultOpen 32

#define NUM_TEXTURE_EXTENSIONS 5
const char* texturesFileExtensions[NUM_TEXTURE_EXTENSIONS] = {
	".png",
	".jpg",
	".jpeg",
	".bmp",
	".tga"
};

namespace Vertex {

	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(Scene* scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(Scene* scene)
	{
		m_Context = scene;
		m_SelectionContext = nullptr;
	}
	char ent_name_buffer[256];
	char ent_type_buffer[256];
	void SceneHierarchyPanel::OnImGuiRender()
	{
		if (ImGuiLink::IsMouseDown(0) && ImGuiLink::IsWindowHovered())
			m_SelectionContext = nullptr;

		if (ImGuiLink::BeginMenuBar())
		{
			if (ImGuiLink::BeginMenu("Entitys"))
			{
				if (ImGuiLink::MenuItem("Create Entity"))
				{
					memset(ent_name_buffer, 0, sizeof(ent_name_buffer));
					strcpy_s(ent_name_buffer, sizeof(ent_name_buffer), "name");

					memset(ent_type_buffer, 0, sizeof(ent_type_buffer));
					strcpy_s(ent_type_buffer, sizeof(ent_type_buffer), "prop_static_sprite");

					m_CreateEntityPopup = true;
				}
				ImGuiLink::EndMenu();
			}
			
				
			ImGuiLink::EndMenuBar();
		}

		ImGuiLink::Begin("Scene Hierarchy");
		for (Entity* ent : *m_Context)
		{
			DrawEntityNode(ent);
		}
		ImGuiLink::End();

		ImGuiLink::Begin("Properties");
		if (m_SelectionContext)
			DrawEntity(m_SelectionContext);
		ImGuiLink::End();

		if (m_CreateEntityPopup)
		{
			ImGuiLink::Begin("Entity Maker", &m_CreateEntityPopup);
			
			ImGuiLink::InputText("Tag", ent_name_buffer, sizeof(ent_name_buffer));
			ImGuiLink::InputText("Entity type", ent_type_buffer, sizeof(ent_type_buffer));

			if (ImGuiLink::Button("Create Entity"))
			{
				m_CreateEntityPopup = false;
				std::string tag = std::string(ent_name_buffer);
				std::string type = std::string(ent_type_buffer);

				if (type == "prop_static_sprite")
				{
					m_Context->CreateEntity<ENTPropStaticSprite>(tag);
				}                       

				if (type == "prop_dynamic_sprite")
				{
					m_Context->CreateEntity<ENTPropDynamicSprite>(tag);
				}

				if (type == "point_camera_2d")
				{
					ENTPointCamera2D* cam = &m_Context->CreateEntity<ENTPointCamera2D>(tag);
					cam->isPrimary = false;
				}
			}

			ImGuiLink::End();

			if (m_EntityToRemove != nullptr)
			{
				m_Context->RemoveEntity(*m_EntityToRemove);
				if (m_SelectionContext == m_EntityToRemove)
					m_SelectionContext = nullptr;
				m_EntityToRemove = nullptr;
			}
		}
	}

	Entity* popupContextEntity = nullptr;
	void SceneHierarchyPanel::DrawEntityNode(Entity* entity)
	{
		
		std::string& tag = entity->name();

		int flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		
		bool opened = ImGuiLink::TreeNodeEx(entity->GetID(), flags, tag);
		
		
		if (ImGuiLink::IsItemClicked())
		{
			m_SelectionContext = entity;
		}
		


		

		if (opened)
		{
			int flags = ImGuiTreeNodeFlags_OpenOnArrow;
			bool opened = ImGuiLink::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGuiLink::TreePop();
			ImGuiLink::TreePop();
		}
	}

#pragma region DrawEntity Code

	void DrawBaseEntity(Entity* entity, Entity** m_EntityToRemove)
	{
		
		auto& tag = entity->name();

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), tag.c_str());
		if (ImGuiLink::InputText("Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
		ImGui::SameLine();
		if (ImGuiLink::Button("Delete"))
		{
			*m_EntityToRemove = entity;
		}

		if (ImGuiLink::TreeNodeEx(entity->GetID() + "Transform", ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
		{
			ImGuiLink::DrawVec3Control("Position", entity->pos);
			
			ImGuiLink::DrawVec3Control("Rotation", entity->rotation);
			
			ImGuiLink::DrawVec3Control("Size", entity->size, 1.0f);
			
			ImGuiLink::TreePop();
		}
	}

	void DrawCameraProp(ENTPointCamera2D* entity)
	{
		if (ImGuiLink::TreeNodeEx((void*)typeid(ENTPointCamera2D).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Point Camera 2D"))
		{
			auto& camera = entity->camera;
			ImGuiLink::Checkbox("Primary", &entity->isPrimary);

			 char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			 char* currentProjectionTypeString = projectionTypeStrings[(int)camera->GetProjectionType()];

			 if (ImGuiLink::BeginCombo("Projection", currentProjectionTypeString))
			 {
				 for (int i = 0; i < 2; i++)
				 {
					 bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					 if (ImGuiLink::Selectable(projectionTypeStrings[i], &isSelected))
					 {
						 currentProjectionTypeString = projectionTypeStrings[i];
						 camera->SetProjectionType((SceneCamera::ProjectionType)i);
					 }
				 }
				 ImGuiLink::EndCombo();
			 }
			 if (camera->GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			 {
				 float verticalFov = glm::degrees(camera->GetPerspectiveVerticalFOV());
				 if (ImGuiLink::DragFloat("Vertical FOV", &verticalFov))
					 camera->SetPerspectiveVerticalFOV(glm::radians(verticalFov));

				 float orthoNear = camera->GetPerspectiveNearClip();
				 if (ImGuiLink::DragFloat("Near", &orthoNear))
					 camera->SetPerspectiveNearClip(orthoNear);

				 float orthoFar = camera->GetPerspectiveFarClip();
				 if (ImGuiLink::DragFloat("Far", &orthoFar))
					 camera->SetPerspectiveFarClip(orthoFar);
			 }
			 if (camera->GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			 {
				 float orthoSize = camera->GetOrthographicSize();
				 if (ImGuiLink::DragFloat("Size", &orthoSize))
					 camera->SetOrthographicSize(orthoSize);

				 float orthoNear = camera->GetOrthographicNearClip();
				 if (ImGuiLink::DragFloat("Near", &orthoNear))
					 camera->SetOrthographicNearClip(orthoNear);

				 float orthoFar = camera->GetOrthographicFarClip();
				 if (ImGuiLink::DragFloat("Far", &orthoFar))
					 camera->SetOrthographicFarClip(orthoFar);
			 }

			 ImGuiLink::TreePop();
		}
	}

	void DrawStaticDynamicRendererProp(ENTPropDynamicSprite* entity)
	{
		if (ImGuiLink::TreeNodeEx((void*)typeid(ENTPropDynamicSprite).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
		{

			ImGuiLink::ColorEdit4("Color", glm::value_ptr(entity->colour));


			ImGuiLink::Button("Texture", glm::vec2(100.0f, 0.0f));
			if (ImGuiLink::BeginDragDropTarget())
			{
				if (const ImGuiLink::ImGuiPayload* payload = ImGuiLink::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					std::string extension = texturePath.extension().string();

					bool isValidExtension = false;

					for (int i = 0; i < NUM_TEXTURE_EXTENSIONS; ++i) {
						if (extension == texturesFileExtensions[i]) {
							isValidExtension = true;
							break;
						}
					}

					if (isValidExtension)
					{
						entity->texture = Texture2D::Create(texturePath.string());
					}

				}
				ImGuiLink::EndDragDropTarget();
			}
			ImGuiLink::DragFloat("Tiling Factor", &entity->tilingFactor, 0.1f, 0.0f, 100.0f);

			ImGuiLink::TreePop();
		}
	}

	void DrawStaticSpriteRendererProp(ENTPropStaticSprite* entity)
	{
		if (ImGuiLink::TreeNodeEx((void*)typeid(ENTPropStaticSprite).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
		{
			
			ImGuiLink::ColorEdit4("Color", glm::value_ptr(entity->colour));


			ImGuiLink::Button("Texture", glm::vec2(100.0f, 0.0f));
			if (ImGuiLink::BeginDragDropTarget())
			{
				if (const ImGuiLink::ImGuiPayload* payload = ImGuiLink::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
					std::string extension = texturePath.extension().string();
					
					bool isValidExtension = false;

					for (int i = 0; i < NUM_TEXTURE_EXTENSIONS; ++i) {
						if (extension == texturesFileExtensions[i]) {
							isValidExtension = true;
							break;
						}
					}

					if (isValidExtension)
					{
						entity->texture = Texture2D::Create(texturePath.string());
					}

				}
				ImGuiLink::EndDragDropTarget();
			}
			ImGuiLink::DragFloat("Tiling Factor", &entity->tilingFactor, 0.1f, 0.0f, 100.0f);

			ImGuiLink::TreePop();
		}
	}

	void DrawBoxCollider2DImGui(ENTBaseBoxCollier2D* ent)
	{
		if (ImGuiLink::TreeNodeEx((void*)typeid(ENTBaseBoxCollier2D).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Box Collider 2D"))
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(ent->Offset));
			ImGui::DragFloat("Density", &ent->Density, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Friction", &ent->Friction, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution", &ent->Restitution, 0.01f, 0.0f, 1.0f);
			ImGui::DragFloat("Restitution Threshold", &ent->RestitutionThreshold, 0.01f, 0.0f);
			ImGuiLink::TreePop();
		}
	}

	void DrawRB2DImGui(ENTBaseRigidbody2D* ent)
	{
		if (ImGuiLink::TreeNodeEx((void*)typeid(ENTBaseRigidbody2D).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Rigidbody 2D"))
		{
			
			if (!(ent->GetEntName() == "prop_static_sprite" || ent->GetEntName() == "prop_dynamic_sprite"))
			{
				const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
				const char* currentBodyTypeString = bodyTypeStrings[(int)ent->Type];
				if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
						if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
						{
							currentBodyTypeString = bodyTypeStrings[i];
							ent->Type = (ENTBaseRigidbody2D::BodyType)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}

			ImGui::Checkbox("Fixed Rotation", &ent->FixedRotation);
			ImGuiLink::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawEntity(Entity* entity)
	{
		if (entity == nullptr) return;
		DrawBaseEntity(entity, &m_EntityToRemove);
		if (entity->GetEntName() == "point_camera_2d") { DrawCameraProp((ENTPointCamera2D*)entity); }
		if (entity->GetEntName() == "prop_static_sprite") { DrawStaticSpriteRendererProp((ENTPropStaticSprite*)entity); }
		if (entity->GetEntName() == "prop_dynamic_sprite") { DrawStaticDynamicRendererProp((ENTPropDynamicSprite*)entity); }
		if (auto boxCollider2D = dynamic_cast<ENTBaseBoxCollier2D*>(entity))
		{
			DrawBoxCollider2DImGui(boxCollider2D);
		}
		if (auto rb2D = dynamic_cast<ENTBaseRigidbody2D*>(entity))
		{
			DrawRB2DImGui(rb2D);
		}
	}
#pragma endregion


	


}