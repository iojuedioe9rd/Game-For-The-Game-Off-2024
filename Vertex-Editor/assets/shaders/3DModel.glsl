#type vertex
#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec4 a_Colour;
layout (location = 3) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Proj;


// Outputs the current position for the Fragment Shader
out vec3 crntPos;
// Outputs the normal for the Fragment Shader
out vec3 Normal;
// Outputs the color for the Fragment Shader
out vec3 v_color;
// Outputs the texture coordinates to the Fragment Shader
out vec2 texCoord;

void main()
{
	gl_Position = vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0) * u_Model * u_View * u_Proj;
	// Assigns the normal from the Vertex Data to "Normal"
	Normal = a_Normal;
	// Assigns the colors from the Vertex Data to "color"
	v_color = a_Colour.xyz;
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = a_TexCoord;
	
	// Outputs the positions/coordinates of all vertices
	gl_Position = (u_View * u_Proj) * vec4(crntPos, 1.0);
}

#type fragment
#version 330 core

out vec4 fragColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 v_color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the color of the light from the main function
uniform vec4 u_LightColor;
// Gets the position of the light from the main function
uniform vec3 u_LightPos;
// Gets the position of the camera from the main function
uniform vec3 u_CamPos;

uniform sampler2D u_Diffuse0;
uniform sampler2D u_Specular0;

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(u_LightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(u_CamPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(u_Diffuse0, texCoord) * (diffuse * inten + ambient) + texture(u_Specular0, texCoord).r * specular * inten) * u_LightColor;
}

void main()
{
	
	fragColor = vec4(texCoord, 1, 1);
}