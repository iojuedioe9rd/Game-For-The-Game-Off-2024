#pragma once
#include "Vertex/Core/Application.h"

void Discord_Register(const char* applicationId, const char* command)
{
	Ref<::Vertex::DynamicLibraryInstance> instance = ::Vertex::Application::Get().GetDLLInstance("plugins\\discord-rpc.dll");

	auto func = instance->getFunction<void(*)(const char*, const char*)>("Discord_Register");

	func(applicationId, command);
}