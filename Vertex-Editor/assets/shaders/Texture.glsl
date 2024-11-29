#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_TexIndex; // Add flat qualifier
out float v_TilingFactor;
flat out int v_EntityID; // Add flat qualifier

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = int(a_TexIndex);
    v_TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_TexIndex; // Add flat qualifier
in float v_TilingFactor;
flat in int v_EntityID; // Add flat qualifier
// Output color
out vec4 color;

uniform mat4 u_ViewProjection;

// Textures
uniform sampler2D u_Textures[10]; // Adjust size as necessary

// CRT Effect Parameters
uniform float u_ScanlineIntensity; // Control scanline darkness
uniform float u_ScanlineWidth;      // Control scanline width
uniform float u_BloomStrength;      // Control bloom effect
uniform float u_CRTDistortion;      // Control CRT distortion

uniform float u_Time;

// Color uniforms
uniform vec4 u_Color1; // Color1
uniform vec4 u_Color2; // Color2
uniform vec4 u_Color3; // Color3

// Function to apply the CRT effect
vec4 applyCRTEffect(vec4 color, vec2 uv)
{
    // Generate scanlines
    float scanline = 0.5 + 0.5 * sin(uv.y * u_ScanlineWidth * 3.14159265358979323846264338327950288419716939937510 + u_Time);
    color.rgb *= scanline * u_ScanlineIntensity;

    // Bloom effect (simple version)
    vec4 bloomColor = texture(u_Textures[v_TexIndex], uv + u_BloomStrength);
    color += bloomColor * 0.5; // Combine with original color

    // Distortion effect to mimic CRT curvature
    vec2 distortedUV = uv + vec2(sin(uv.y * 10.0) * u_CRTDistortion, 0.0);
    vec4 distortedColor = texture(u_Textures[v_TexIndex], distortedUV);
    
    // Mix the distorted color into the final color
    color = mix(color, distortedColor, 0.1); // Blend the distorted color lightly

    return color;
}

// Function for edge detection
float edgeDetection(vec2 uv)
{
    // Sample neighboring pixels for edge detection
    vec4 center = texture(u_Textures[v_TexIndex], uv);
    vec4 north = texture(u_Textures[v_TexIndex], uv + vec2(0.0, 0.01));
    vec4 south = texture(u_Textures[v_TexIndex], uv + vec2(0.0, -0.01));
    vec4 east = texture(u_Textures[v_TexIndex], uv + vec2(0.01, 0.0));
    vec4 west = texture(u_Textures[v_TexIndex], uv + vec2(-0.01, 0.0));

    float edgeStrength = 0.0;

    // Calculate the differences
    edgeStrength += abs(center.r - north.r);
    edgeStrength += abs(center.r - south.r);
    edgeStrength += abs(center.r - east.r);
    edgeStrength += abs(center.r - west.r);

    return edgeStrength;
}

// Dithering function
void Unity_Dither_float4(vec4 In, vec2 ScreenPosition, out vec4 Out)
{
    float DITHER_THRESHOLDS[16] = float[16](
        1.0 / 17.0,  9.0 / 17.0,  3.0 / 17.0, 11.0 / 17.0,
        13.0 / 17.0,  5.0 / 17.0, 15.0 / 17.0,  7.0 / 17.0,
        4.0 / 17.0, 12.0 / 17.0,  2.0 / 17.0, 10.0 / 17.0,
        16.0 / 17.0,  8.0 / 17.0, 14.0 / 17.0,  6.0 / 17.0
    );

    // Determine the index for the dither matrix based on fragment coordinates
    int index = (int(mod(ScreenPosition.x, 4.0)) * 4 + int(mod(ScreenPosition.y, 4.0)));

    // Subtract the threshold from the input color
    Out = In - vec4(DITHER_THRESHOLDS[index]);
}

// Simple hash function to generate pseudo-random values
float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

// Function to get noise based on position
float noise(vec2 uv) {
    return (hash(uv.x * 100.0) + hash(uv.y * 100.0)) * 0.5; // Combine hash functions for more noise
}


void main()
{
    // Sample the texture color
    vec4 texColor = texture(u_Textures[v_TexIndex], v_TexCoord * v_TilingFactor) * v_Color;

    if(texColor.a <= 0.01)
    {
        discard; // Discard fragments with very low alpha
    }

    texColor = applyCRTEffect(texColor, v_TexCoord * v_TilingFactor);


    // Calculate brightness as the average of RGB values
    float noiseValue = noise(gl_FragCoord.xy + u_ViewProjection[0].xy); // Generate noise based on fragment coordinates
    float brightness = (texColor.r + texColor.g + texColor.b) / 3.0;
    brightness += (noiseValue - 0.5) * 0.1;

    // Perform edge detection
    float edgeStrength = edgeDetection(v_TexCoord);
    float edgeThreshold = 0.1 + ((noiseValue - 0.5) * 0.1); // Adjust this threshold based on the desired sensitivity
    float edgeEffect = step(edgeThreshold, edgeStrength); // Create a mask based on edge detection

    // Dither based on brightness
    vec4 ditheredColor;
    vec2 screenPosition = gl_FragCoord.xy  + u_ViewProjection[0].xy; // Use fragment coordinates directly
    Unity_Dither_float4(texColor, screenPosition, ditheredColor);

    float factorA = 0.3; // 30% of colorA
    float factorB = 0.4; // 40% of colorB
    float factorC = 0.3; // 30% of colorC

    // Ensure the factors sum to 1.0 (this is a good practice)
    vec3 mixedColor = mix(u_Color1.rgb * factorA, mix(u_Color2.rgb * factorB, u_Color3.rgb * factorC, factorC / (factorB + factorC)), factorB / (factorA + factorB + factorC));

    // Output color based on brightness thresholds
    vec4 finalColor;
    if (brightness < ditheredColor.r) {
        finalColor = u_Color1; // Use COLOR1
    } else if (brightness < ditheredColor.g) {
        finalColor = u_Color2; // Use COLOR2
    } else {
        finalColor = u_Color3; // Use COLOR3
    }

    vec4 temp = finalColor;

    // If edge effect is detected, modify the output color (for example, darken the color)
    finalColor.rgb = mix(finalColor.rgb, mixedColor, edgeEffect); // Darken edges

    if(edgeEffect > 0.0)
    {
        finalColor.rgb = mix(temp.rgb, vec3(0,0,0), 0.85);
    }

    // Preserve the alpha channel from dithered color
    finalColor.a = ditheredColor.a;

    // Output the final color
    color = finalColor;
}
