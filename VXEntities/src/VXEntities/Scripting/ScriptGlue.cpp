#include "ScriptGlue.h"

#include "mono/metadata/object.h"
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <Vertex/Core/Logger.h>
#include "../Scene/Entity.h"
#include "../../VXEntities.h"
#include <Vertex/Renderer/Renderer2D.h>
#include "ScriptEngine.h"
#include <Vertex/Core/KeyCodes.h>
#include <Vertex/Core/Input.h>

#include "mono/metadata/reflection.h"
#include "box2d/b2_body.h"

#include "Vertex/Renderer/TextureManager.h"
#include "../Scene/Entities/Entities.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include <mono/metadata/appdomain.h>

static b2BodyType Rigidbody2DTypeToBox2DBody(Vertex::ENTBaseRigidbody2D::BodyType bodyType)
{
	switch (bodyType)
	{
	case Vertex::ENTBaseRigidbody2D::BodyType::Static:    return b2_staticBody;
	case Vertex::ENTBaseRigidbody2D::BodyType::Dynamic:   return b2_dynamicBody;
	case Vertex::ENTBaseRigidbody2D::BodyType::Kinematic: return b2_kinematicBody;
	}
	VX_CORE_ASSERT(false, "Unknown body type");
	return b2_staticBody;
}

namespace Vertex
{
	

#define VX_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Vertex.InternalCalls::" #Name, Name)

#define CORE_LOG_FLAG (1 << 0)    // 0b000001 = 1
#define LOG_TRACE     (1 << 1)    // 0b000010 = 2
#define LOG_INFO      (1 << 2)    // 0b000100 = 4
#define LOG_WARN      (1 << 3)    // 0b001000 = 8
#define LOG_ERROR     (1 << 4)    // 0b010000 = 16
#define LOG_CRITICAL  (1 << 5)    // 0b100000 = 32

	static void Input_GetMousePosWorld(glm::vec2* Pos)
	{
		glm::vec2 mousePos = Input::GetMousePositionVec2();

		for (Entity* ent : *ScriptEngine::GetSceneContext())
		{
			if (ent->GetEntName() == "point_camera_2d")
			{
				ENTPointCamera2D* cam = (ENTPointCamera2D*)ent;
				Ref<SceneCamera> camera = cam->camera;
				*Pos = camera->ScreenToWorldPoint(mousePos);
			}
		}
	}

	static void Input_GetMousePos(glm::vec2* Pos)
	{
		*Pos = Input::GetMousePositionVec2();
	}
	
	//internal extern static bool Input_IsMouseButtonPressed(MouseCode button);
	bool Input_IsMouseButtonPressed(MouseCode button)
	{
		return Input::IsMouseButtonPressed(button);
	}

	static MonoString* Entity_FindEntityByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);
		Scene* scene = ScriptEngine::GetSceneContext();
		VX_CORE_ASSERT(scene);
		Entity* entity = scene->FindEntityByName(nameCStr);
		mono_free(nameCStr);
		if (!entity)
			return mono_string_empty(ScriptEngine::GetAppDomain());

		return mono_string_new(ScriptEngine::GetAppDomain(), entity->GetID().c_str());
	}

	static MonoArray* Entity_FindEntitiesByName(MonoString* name)
	{
		char* nameCStr = mono_string_to_utf8(name);
		Scene* scene = ScriptEngine::GetSceneContext();
		std::vector<Entity*> entities = scene->FindEntitiesByName(nameCStr, "env_script");
		mono_free(nameCStr);

		MonoArray* array = mono_array_new(ScriptEngine::GetAppDomain(), mono_get_string_class(), entities.size());

		for (size_t i = 0; i < entities.size(); ++i) 
		{
			std::string entityId = entities[i]->GetID();
			MonoString* monoEntityId = mono_string_new(ScriptEngine::GetAppDomain(), entityId.c_str());
			mono_array_set(array, MonoString*, i, monoEntityId);
		}

		return array;

	}

	MonoClass* RB2DClass = nullptr;
	MonoClass* BC2DClass = nullptr;

	// internal extern static string Entity_NewEntity(string FullName, string name, ref Vector3 translation, ref Vector3 size, ref Vector3 rotation);
	static MonoString* Entity_NewEntity(MonoString* FullName, MonoString* name, glm::vec3* translation, glm::vec3* size, glm::vec3* rotation)
	{
		char* FullNameCStr = mono_string_to_utf8(FullName);
		char* nameCStr = mono_string_to_utf8(name);
		Scene* scene = ScriptEngine::GetSceneContext();
		ENTEnvScript* sc = &scene->CreateEntity<ENTEnvScript>(std::string(nameCStr));
		sc->classname = std::string(FullNameCStr);
		sc->pos = *translation;
		sc->size = *size;
		sc->rotation = *rotation;

		ScriptEngine::OnCreateEntity(sc, [&](ENTEnvScript* sc) { return true; });

		RB2DClass = ScriptEngine::GetMonoClassFromName(ScriptEngine::GetCoreAssemblyImage(), "Vertex", "ENTBaseRigidbody2D");
		BC2DClass = ScriptEngine::GetMonoClassFromName(ScriptEngine::GetCoreAssemblyImage(), "Vertex", "ENTBaseBoxCollier2D");

		Ref<ScriptInstance> Instance = ScriptEngine::GetEntityInstance(sc->GetID());

		MonoClass* monoClass = (MonoClass*)Instance->GetScriptClass()->GetMonoClass();

		if (ScriptEngine::IsSubclassOf(monoClass, BC2DClass, false))
		{
			MonoClassField* bodyTypeField = mono_class_get_field_from_name(RB2DClass, "Type");
			int32_t bodyTypeEnumValue;
			mono_field_get_value(Instance->GetManagedObject(), bodyTypeField, &bodyTypeEnumValue); // Pass the address of the variable

			MonoClassField* fixedRotationField = mono_class_get_field_from_name(RB2DClass, "FixedRotation");
			bool FixedRotation;
			mono_field_get_value(Instance->GetManagedObject(), fixedRotationField, &FixedRotation); // Pass the address of the variable

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody((ENTBaseRigidbody2D::BodyType)bodyTypeEnumValue);
			bodyDef.position.Set(sc->pos.x, sc->pos.y);

			bodyDef.angle = sc->rotation.z;

			b2Body* body = ScriptEngine::GetSceneContext()->GetPhysicsWorld()->CreateBody(&bodyDef);
			body->SetFixedRotation(FixedRotation);



			MonoClassField* bodyPtrField = mono_class_get_field_from_name(RB2DClass, "bodyPtr");



			mono_field_set_value(Instance->GetManagedObject(), bodyPtrField, (void*)(&body));

			b2PolygonShape boxShape;

			boxShape.SetAsBox(0.5f * sc->size.x, 0.5f * sc->size.x);

			b2FixtureDef fixtureDef;
			fixtureDef.shape = &boxShape;

			MonoClassField* densityField = mono_class_get_field_from_name(monoClass, "Density");
			mono_field_get_value(Instance->GetManagedObject(), densityField, &fixtureDef.density); // Pass the address of the variable

			// Friction
			MonoClassField* frictionField = mono_class_get_field_from_name(monoClass, "Friction");
			mono_field_get_value(Instance->GetManagedObject(), frictionField, &fixtureDef.friction); // Pass the address of the variable

			// Restitution
			MonoClassField* restitutionField = mono_class_get_field_from_name(monoClass, "Restitution");
			mono_field_get_value(Instance->GetManagedObject(), restitutionField, &fixtureDef.restitution); // Pass the address of the variable

			// Restitution
			MonoClassField* restitutionThresholdField = mono_class_get_field_from_name(monoClass, "RestitutionThreshold");
			mono_field_get_value(Instance->GetManagedObject(), restitutionThresholdField, &fixtureDef.restitutionThreshold); // Pass the address of the variable

			body->CreateFixture(&fixtureDef);

		}


		return mono_string_new(ScriptEngine::GetAppDomain(), sc->GetID().c_str());
	}

	static bool Entity_RemoveEntity(MonoString* entityID)
	{
		char* entityIDCStr = mono_string_to_utf8(entityID);

		Scene* scene = ScriptEngine::GetSceneContext();

		Entity* ent = scene->FindEntityByID(entityIDCStr);

		scene->RemoveEntity(*ent);

		return ent == 0 ? 0 : 1;
	}

	static MonoObject* GetScriptInstance(MonoString* entityID)
	{
		char* nameCStr = mono_string_to_utf8(entityID);
		return ScriptEngine::GetManagedInstance(std::string(nameCStr));
	}

	

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyDown(keycode);
	}

	static bool Input_IsKeyUp(KeyCode keycode)
	{
		return Input::IsKeyUp(keycode);
	}

	// Texture2D_FromFilename

	void Texture2D_FromFilename(MonoString* string)
	{
		static Ref<Texture2D>* Wat = nullptr;
		const char* cStr = mono_string_to_utf8(string);
		
		if (Wat) delete Wat;
		Wat = &TextureManager2D::GetOrMakeTextureFromFilename(std::string(cStr));
	}

	// internal extern static void Renderer2D_DrawQuad(ref Vector3 pos, ref Vector3 size, string textureFilename, float tilingFactor = 1.0f, ref Colour tintColour = new Colour(1,1,1,1));
	void Renderer2D_DrawQuadTex(glm::vec3* pos, glm::vec3* size, MonoString* textureFilename, float tilingFactor, glm::vec4* tintColour)
	{
		const char* cStr = mono_string_to_utf8(textureFilename);

		Ref<Texture2D> tex = TextureManager2D::GetOrMakeTextureFromFilename(std::string(cStr));

		Renderer2D::DrawQuad(*pos, glm::vec2(size->x, size->y), tex, tilingFactor, *tintColour);
	}

	void Renderer2D_DrawQuad(glm::vec3* pos, glm::vec3* size, glm::vec4* colour)
	{
		Renderer2D::DrawQuad(*pos, glm::vec2(size->x, size->y), *colour);
	}

	std::string logBuffer;
	int logFlags;

	static void BeginLog(int flags)
	{
		logBuffer = std::string();
		logFlags = flags;
	}

	static void LogMsg(MonoString* string)
	{
		char* cStr = mono_string_to_utf8(string);
		logBuffer += cStr;
	}

	static void EndLog()
	{
		
		bool is_core_log = logFlags & CORE_LOG_FLAG;
		if (logFlags & LOG_TRACE)
		{
			if (is_core_log)
			{
				VX_CORE_TRACE("{}", logBuffer.c_str());
				return;
			}
			else
			{
				VX_TRACE("{}", logBuffer.c_str());
			}
		}
		if (logFlags & LOG_INFO)
		{
			if (is_core_log)
			{
				VX_CORE_INFO("{}", logBuffer.c_str());
				return;
			}
			else
			{
				VX_INFO("{}", logBuffer.c_str());
			}
		}
		if (logFlags & LOG_WARN)
		{
			if (is_core_log)
			{
				VX_CORE_WARN("{}", logBuffer.c_str());
				return;
			}
			else
			{
				VX_WARN("{}", logBuffer.c_str());
			}
		}
		if (logFlags & LOG_ERROR)
		{
			if (is_core_log)
			{
				VX_CORE_ERROR("{}", logBuffer.c_str());
				return;
			}
			else
			{
				VX_ERROR("{}", logBuffer.c_str());
			}
		}
		if (logFlags & LOG_CRITICAL)
		{
			if (is_core_log)
			{
				VX_CORE_CRITICAL("{}", logBuffer.c_str());
				return;
			}
			else
			{
				VX_CRITICAL("{}", logBuffer.c_str());
			}
		}

			
	}

	static void Entity_GetTranslation(MonoString* entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VX_CORE_ASSERT(scene);
		Entity* entity = nullptr;
		char* cStr = mono_string_to_utf8(entityID);
		for (Entity* ent : *scene)
		{
			
			if (ent->GetID() == std::string(cStr))
			{
				entity = ent;
				break;
			}
		}
		//VX_CORE_ASSERT(entity);
		
		if(entity)
			*outTranslation = entity->pos;
	}

	static MonoString* Object_GenerateUUID()
	{
		MonoString* str = mono_string_new(ScriptEngine::GetAppDomain(), get_uuid().data());
		return str;
	}

	static void Rigidbody2D_ApplyLinearImpulse(b2Body** body, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		(*body)->ApplyLinearImpulse(b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2D_ApplyLinearImpulseToCenter(b2Body** body, glm::vec2* impulse, bool wake)
	{
		(*body)->ApplyLinearImpulseToCenter(b2Vec2(impulse->x, impulse->y), wake);
	}

	static void Rigidbody2D_GetTransform(b2Body** body, glm::vec4* transform2D)
	{
		const auto& position = (*body)->GetPosition();
		
		transform2D->x = position.x;
		transform2D->y = position.y;
		transform2D->w = glm::degrees((*body)->GetAngle());
	}

	static void Rigidbody2D_SetTransform(b2Body** body, glm::vec4* transform2D)
	{
		(*body)->SetTransform(b2Vec2(transform2D->x, transform2D->y), glm::radians(transform2D->w));
	}

	


	static void Entity_SetTranslation(MonoString* entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		VX_CORE_ASSERT(scene);
		Entity* entity = nullptr;
		for (Entity* ent : *scene)
		{
			char* cStr = mono_string_to_utf8(entityID);
			if (ent->GetID() == std::string(cStr))
			{
				entity = ent;
				break;
			}
		}
		//VX_CORE_ASSERT(entity);
		if (entity)
			entity->pos = *translation;
	}

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}
	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		glm::vec3 NewParameter = *parameter;
		VX_CORE_WARN("Value: {0}", NewParameter);
		*outResult = glm::normalize(NewParameter);
	}
	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		glm::vec3 NewParameter = *parameter;
		VX_CORE_WARN("Value: {0}", NewParameter);
		return glm::dot(NewParameter, NewParameter);
	}

	template<typename... ENT>
	static void RegisterENT()
	{
		([]()
			{
				std::string_view typeName = typeid(ENT).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Vertex.{}", structName);
				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					VX_CORE_ERROR("	Could not find ENT type {}", managedTypename);
					return;
				}
				
			}(), ...);
	}
	template<typename... ENT>
	static void RegisterENT(EntityGroup<ENT...>)
	{
		VX_CORE_INFO("Registering Entities");
		RegisterENT<ENT...>();
		VX_CORE_INFO("~Registering Entities");
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterENT(AllEntities{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		VX_ADD_INTERNAL_CALL(Input_GetMousePosWorld);
		VX_ADD_INTERNAL_CALL(Input_GetMousePos);
		VX_ADD_INTERNAL_CALL(Input_IsMouseButtonPressed);

		VX_ADD_INTERNAL_CALL(NativeLog);
		VX_ADD_INTERNAL_CALL(NativeLog_Vector);
		VX_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		VX_ADD_INTERNAL_CALL(BeginLog);
		VX_ADD_INTERNAL_CALL(LogMsg);
		VX_ADD_INTERNAL_CALL(EndLog);

		VX_ADD_INTERNAL_CALL(Renderer2D_DrawQuad);
		VX_ADD_INTERNAL_CALL(Texture2D_FromFilename);
		VX_ADD_INTERNAL_CALL(Renderer2D_DrawQuadTex);

		VX_ADD_INTERNAL_CALL(GetScriptInstance);

		VX_ADD_INTERNAL_CALL(Entity_GetTranslation);
		VX_ADD_INTERNAL_CALL(Entity_SetTranslation);
		VX_ADD_INTERNAL_CALL(Entity_FindEntityByName);
		VX_ADD_INTERNAL_CALL(Entity_NewEntity);
		VX_ADD_INTERNAL_CALL(Entity_RemoveEntity);
		VX_ADD_INTERNAL_CALL(Entity_FindEntitiesByName);

		VX_ADD_INTERNAL_CALL(Input_IsKeyDown);
		VX_ADD_INTERNAL_CALL(Input_IsKeyUp);

		VX_ADD_INTERNAL_CALL(Rigidbody2D_ApplyLinearImpulse);
		VX_ADD_INTERNAL_CALL(Rigidbody2D_ApplyLinearImpulseToCenter);
		VX_ADD_INTERNAL_CALL(Rigidbody2D_GetTransform);
		VX_ADD_INTERNAL_CALL(Rigidbody2D_SetTransform);

		VX_ADD_INTERNAL_CALL(Object_GenerateUUID);
	}

}