#include "vxpch.h"
#include "Assert.h"
#include "Vertex/Core/Application.h"

void Assert_INTERNAL_Close()
{
	Vertex::Application::Get().Close();
}
