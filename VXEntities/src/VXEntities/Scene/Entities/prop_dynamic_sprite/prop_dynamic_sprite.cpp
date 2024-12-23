﻿#include "prop_dynamic_sprite.h"
#include "Vertex/Renderer/Renderer2D.h"
#include "Vertex/Math/Math.h"

namespace Vertex
{
	ENTPropDynamicSprite::~ENTPropDynamicSprite()
	{
	}
	void ENTPropDynamicSprite::Update(Timestep& ts)
	{
		// Do nothing but if we don't define it won't work ╰（‵□′）╯
	}

	void ENTPropDynamicSprite::Draw(Timestep& ts)
	{
		if (Type != ENTBaseRigidbody2D::BodyType::Dynamic)
		{
			Type = ENTBaseRigidbody2D::BodyType::Dynamic;
		}
		int id = 0;
		for (char c : GetID())
		{
			id += c;
		}
		glm::mat4 mat = Math::ComposeTransform(pos, size, rotation);

		if (texture)
		{
			Renderer2D::DrawMatrix(mat, texture, tilingFactor, colour, id);
		}
		else
		{
			Renderer2D::DrawMatrix(mat, colour, id);
		}

	}
}
