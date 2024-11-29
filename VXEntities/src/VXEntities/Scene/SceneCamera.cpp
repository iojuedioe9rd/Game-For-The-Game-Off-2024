#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Vertex {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}
	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0) return;
		m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	glm::vec2 SceneCamera::ScreenToWorldPoint(glm::vec2 screenPoint)
	{
		// Step 1: Convert screen coordinates to normalized device coordinates (NDC)
		glm::vec2 normalizedScreenPoint;
		normalizedScreenPoint.x = (2.0f * screenPoint.x) / m_AspectRatio - 1.0f;
		normalizedScreenPoint.y = 1.0f - (2.0f * screenPoint.y) / m_AspectRatio;

		// Step 2: Create a clip space position from the NDC, assuming z = -1 for 2D screen space
		glm::vec4 clipSpacePos = glm::vec4(normalizedScreenPoint, -1.0f, 1.0f);

		// Step 3: Invert the projection matrix to transform clip space to world space
		glm::mat4 inverseProjection = glm::inverse(m_Projection);

		// Step 4: Apply the inverse projection to get the world coordinates
		glm::vec4 worldCoords = inverseProjection * clipSpacePos;

		// Step 5: Return the x, y components of the transformed world position
		return glm::vec2(worldCoords.x, worldCoords.y);
	}

	void SceneCamera::RecalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}
	}

}