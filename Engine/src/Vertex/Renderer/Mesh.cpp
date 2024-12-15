#include "vxpch.h"
#include "Mesh.h"
#include "RenderCommand.h"

namespace Vertex
{
	Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, std::vector<MaterialTexture>& textures)
	{
		Mesh::vertices = vertices;
		Mesh::indices = indices;
		Mesh::textures = textures;

		std::vector<float> verticesA;
		verticesA.reserve(vertices.size() * 12); // Pre-reserve space for performance

		for (const Vertex& vertex : vertices)
		{
			verticesA.insert(verticesA.end(), {
				vertex.position.x, vertex.position.y, vertex.position.z, // Position
				vertex.normal.x, vertex.normal.y, vertex.normal.z,       // Normal
				vertex.colour.x, vertex.colour.y, vertex.colour.z, vertex.colour.w, // Colour
				vertex.texUV.x, vertex.texUV.y                           // UV
				});
		}

		Ref<VertexBuffer> vertexBuffer = Ref<VertexBuffer>(VertexBuffer::Create(verticesA.data(), verticesA.size() * sizeof(float)));
		Ref<IndexBuffer> indexBuffer = Ref<IndexBuffer>(IndexBuffer::Create(indices.data(), indices.size()));

		VAO = Ref<VertexArray>(VertexArray::Create());
		vertexBuffer->SetLayout({
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float3, "a_Normal" },
			{ ShaderDataType::Float4, "a_Colour" },
			{ ShaderDataType::Float2, "a_TexUV" },
			});

		VAO->AddVertexBuffer(std::move(vertexBuffer));
		VAO->SetIndexBuffer(std::move(indexBuffer));
	}


	void Mesh::Draw(Ref<Shader> shader)
	{
		shader->Bind();
		VAO->Bind();

		BaseDraw(shader);

		VAO->Unbind();
		shader->Unbind();
	}

	void Mesh::Draw(Ref<Shader> shader, glm::vec3 camPos, glm::mat4 camMatrix)
	{
		shader->Bind();
		VAO->Bind();

		shader->UploadUniformFloat3("u_CamPos", camPos);
		shader->UploadUniformMat4("u_CamMatrix", camMatrix);

		BaseDraw(shader);

		VAO->Unbind();
		shader->Unbind();
	}
	void Mesh::BaseDraw(Ref<Shader> shader)
	{
		unsigned int numDiffuse = 0;
		unsigned int numSpecular = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			std::string num = "lol";
			std::string type = textures[i].type;
			if (type == "diffuse")
			{
				num = std::to_string(numDiffuse++);
			}
			else if (type == "specular")
			{
				num = std::to_string(numSpecular++);
			}
			type[0] = toupper(type[0]);
			std::string name = "u_" + type + num;
			shader->UploadUniformInt(name.c_str(), i);
			textures[i].texture->Bind(i);
		}

		RenderCommand::DrawIndexed(VAO, VAO->GetIndexBuffer()->GetCount());
	}
}