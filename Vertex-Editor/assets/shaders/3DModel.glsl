#type vertex
#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Colour;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;

// Outputs the colour for the Fragment Shader
out vec3 v_Colour;
// Outputs the texture coordinates to the fragment shader
out vec2 v_TexCoord;

out vec3 v_Normal;

void main()
{
	gl_Position = vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0) * u_Model * u_View * u_Proj;
	v_TexCoord = a_TexCoord;
	v_Colour = a_Colour;
	v_Normal = a_Normal;
}

#type fragment
#version 330 core

out vec4 color;

// Outputs the colour for the Fragment Shader
in vec3 v_Colour;
// Outputs the texture coordinates to the fragment shader
in vec2 v_TexCoord;
in vec3 v_Normal;

uniform sampler2D u_Tex0;

void main()
{
	//color = texture(u_Tex0, v_TexCoord) *  vec4(v_Colour, 1.0);
	color = vec4(v_Normal, 1);
}