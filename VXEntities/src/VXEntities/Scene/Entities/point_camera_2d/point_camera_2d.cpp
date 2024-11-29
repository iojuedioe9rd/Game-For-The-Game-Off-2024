#include "point_camera_2d.h"

glm::vec2 m_PlayerMoveDiff = glm::vec2(0, 0);
glm::vec2 m_PlayerOldPos = glm::vec2(0, 0);

float m_MoveSpeed = 4.0f;

float lookAheadDis = 3.0f;
float lookAheadSpeed = 2.0f;

float lookOffset = 0;

namespace Vertex
{
	

	void ENTPointCamera2D::Update(Timestep& ts)
	{
		Entity* player = m_Scene->FindEntityByName("Player");

		

		if (player)
		{
			glm::vec2 targetPoint = glm::vec2(player->pos.x, player->pos.y);

			m_PlayerMoveDiff = m_PlayerOldPos - glm::vec2(player->pos.x, player->pos.y);

			

			



			this->pos.x = targetPoint.x;
			this->pos.y = targetPoint.y;

			m_PlayerOldPos = glm::vec2(player->pos.x, player->pos.y);
		}
		


	}
}
