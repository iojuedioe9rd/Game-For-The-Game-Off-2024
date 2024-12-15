#pragma once
#include <glm/glm.hpp>
#include "Vertex/Core/Base.h"
#include "Vertex/Renderer/Buffer.h"
#include "Vertex/Renderer/VertexArray.h"
#include "Vertex/Renderer/Texture.h"
#include "Shader.h"

namespace Vertex
{
	struct VERTEX_API Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 colour;
		glm::vec2 texUV;
	};

	struct VERTEX_API MaterialTexture
	{
		Ref<Texture2D> texture;
		std::string type;

		MaterialTexture(Ref<Texture2D> texture, std::string type)
		{
			this->texture = texture;
			this->type = type;
		}
	};

	class VERTEX_API Mesh
	{
	public:
		Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<MaterialTexture>& textures);

		void Draw(Ref<Shader> shader);
		void Draw(Ref<Shader> shader, glm::vec3 camPos, glm::mat4 camMatrix);

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<MaterialTexture> textures;

		Ref<VertexArray> VAO;
	private:
		void BaseDraw(Ref<Shader> shader);
		

	};

}



