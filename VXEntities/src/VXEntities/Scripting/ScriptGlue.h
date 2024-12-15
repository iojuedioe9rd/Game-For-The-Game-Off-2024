#pragma once
#include <glm/glm.hpp>

namespace Vertex
{
	class ScriptGlue
	{
	public:
		static void RegisterComponents();
		static void RegisterFunctions();

		static void IsEditor(bool isEditor, glm::vec2 windowSize, glm::vec2 screenSettings[3]);
	};
}