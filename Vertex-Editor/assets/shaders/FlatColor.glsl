// Flat Color Shader

#type vertex
#version 330 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 0.0, 1.0);
}
#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 TexCoord;

uniform sampler2D u_Texture;
uniform vec2 u_Res;
uniform float u_Time;

uniform vec4 u_Color1; // Color1
uniform vec4 u_Color2; // Color2
uniform vec4 u_Color3; // Color3

uniform vec2 u_CamPos; // Color3

vec3 applyAnimatedScanlines(vec3 texColor, vec2 TexCoord, float lineDensity, float time)
{
    // Calculate the scanline intensity with time-based animation
    float line = sin((TexCoord.y + time * 0.5) * lineDensity) * 0.5 + 0.5;

    // Optional flicker effect
    float flicker = sin(time * 10.0) * 0.05 + 0.95; // Flickers between 0.95 and 1.0

    return texColor * line * flicker;
}


// Grayscale effect
vec3 applyGrayscale(vec3 texColor)
{
    float gray = dot(texColor, vec3(0.299, 0.587, 0.114)); // Luma weights
    return vec3(gray);
}

// Sepia effect
vec3 applySepia(vec3 texColor)
{
    return vec3(
        dot(texColor, vec3(0.393, 0.769, 0.189)), 
        dot(texColor, vec3(0.349, 0.686, 0.168)), 
        dot(texColor, vec3(0.272, 0.534, 0.131))
    );
}

// Vignette effect
vec3 applyVignette(vec3 texColor, vec2 TexCoord)
{
    vec2 center = vec2(0.5, 0.5);
    float distance = length(TexCoord - center);
    float vignette = smoothstep(0.7, 1.0, distance);
    return texColor * (1.0 - vignette);
}

// Pixelation effect
vec3 applyPixelation(vec3 texColor, vec2 TexCoord)
{
    float pixelSize = 10.0; // Control the size of the pixelation
    vec2 pixelCoord = floor(TexCoord * u_Res / pixelSize) * pixelSize / u_Res;
    return texture(u_Texture, pixelCoord).rgb;
}

// Invert effect
vec3 applyInvert(vec3 texColor)
{
    return vec3(1.0) - texColor;
}

// Bloom effect
vec3 applyBloom(vec3 texColor)
{
    // Simple bloom by brightening bright areas
    float brightness = max(max(texColor.r, texColor.g), texColor.b);
    vec3 bloomColor = texColor * (brightness > 1.0 ? 0.2 : 0.0);
    return texColor + bloomColor;
}

// Color Grading effect (Brightness and Contrast)
vec3 applyColorGrading(vec3 texColor, float brightness, float contrast)
{
    texColor += vec3(brightness); // Adjust brightness
    texColor = (texColor - 0.5) * contrast + 0.5; // Adjust contrast
    return texColor;
}

// Convolution Kernel (Edge detection)
vec3 applyKernelEffect(vec3 texColor, vec2 texCoord, vec2 res)
{
    // Define a kernel (e.g., edge detection kernel)
    float kernel[9] = float[](
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    );

    // Get the offsets based on the resolution
    float offsetX = 1.0 / 800.0;
    float offsetY = 1.0 / 800.0;

    // Define offsets for the 9 neighboring pixels in the kernel
    vec2 offsets[9] = vec2[](
        vec2(-offsetX, offsetY), vec2(0, offsetY), vec2(offsetX, offsetY),
        vec2(-offsetX, 0), vec2(0, 0), vec2(offsetX, 0),
        vec2(-offsetX, -offsetY), vec2(0, -offsetY), vec2(offsetX, -offsetY)
    );

    // Apply the kernel to the 3x3 neighborhood of texColor
    vec3 resultColor = vec3(0.0);
    for (int i = 0; i < 9; i++)
    {
        // Fetch the neighboring pixel color
        vec3 neighborColor = texture(u_Texture, texCoord + offsets[i]).rgb;
        resultColor += neighborColor * kernel[i];
    }

    // Scale and clamp result for visibility
    resultColor = abs(resultColor); // Take the absolute value to avoid negative colors
    return clamp(resultColor, 0.0, 1.0);
}


// Ray Marching
void render(inout vec3 col, in vec2 uv)
{
    vec3 ro = vec3(0,0,-3);

}

vec3 applyAnisotropicKuwahara(sampler2D textureID, vec2 texCoord, vec2 res, float radius)
{
    vec2 offsets[8] = vec2[](
        vec2(-1, 0), vec2(1, 0), 
        vec2(0, -1), vec2(0, 1), 
        vec2(-0.707, -0.707), vec2(0.707, -0.707), 
        vec2(-0.707, 0.707), vec2(0.707, 0.707)
    );

    vec3 resultColor = vec3(0.0);
    float totalWeight = 0.0;
    
    for (int i = 0; i < 8; i++)
    {
        vec2 offset = offsets[i] * radius / res;
        vec3 sampleColor = texture(textureID, texCoord + offset).rgb;
        
        // Weight based on distance from texCoord and pixel intensity variation
        float weight = 1.0 / (0.1 + length(sampleColor - texture(textureID, texCoord).rgb));
        resultColor += sampleColor * weight;
        totalWeight += weight;
    }
    
    resultColor /= totalWeight; // Normalize by total weight
    return resultColor;
}

// Flip the Y-axis for texture coordinates
//    vec2 flippedTexCoord = vec2(TexCoord.x, 1.0 - TexCoord.y);

vec2 snapToPixelGrid(vec2 uv, vec2 textureResolution) {
    // Calculate the pixel size in UV space
    vec2 pixelSize = 1.0 / textureResolution;
    
    // Snap UVs to the nearest pixel center
    return floor(uv * textureResolution) * pixelSize;
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
    vec2 textureResolution = vec2(256.0, 256.0) + (vec2(noise(TexCoord + u_CamPos), noise(TexCoord + u_CamPos)) * 5.005); // Replace with actual texture resolution

    float offset_x = 1.0 / u_Res.x + (noise(TexCoord) * 0.005);  // Calculate horizontal offset
    float offset_y = 1.0 / u_Res.y + (noise(TexCoord) * 0.005);  // Calculate vertical offset

    vec2 flippedTexCoord = vec2(TexCoord.x, 1.0 - TexCoord.y) + (vec2(noise(TexCoord + u_CamPos), noise(TexCoord + u_CamPos)) * 0.005);
    vec2 snappedUV = snapToPixelGrid(flippedTexCoord, textureResolution);

    vec3 texColor = texture(u_Texture, flippedTexCoord).xyz;

    // Apply kernel effect (convolution/edge detection)
    texColor = applyKernelEffect(texColor, snappedUV, u_Res);
    texColor = applyKernelEffect(texColor, snappedUV, u_Res);
    

    // Apply pixelation
    


    // Apply bloom
    //texColor = applyBloom(texColor);
    float noiseValue = noise(gl_FragCoord.xy + u_CamPos);
    float brightness = (texColor.r + texColor.g + texColor.b) / 3.0;
    brightness += (noiseValue - 0.5) * 0.1;

    texColor *= 1.0 + brightness;
    // Apply color grading (adjustable brightness and contrast)
    //texColor = applyColorGrading(texColor, brightness + (noise(TexCoord) / 100), 1.2); // Example brightness and contrast values
    texColor = applyAnimatedScanlines(texColor, snappedUV, 800.0, u_Time);

    
    
    vec3 finalColor = texColor;
    

    finalColor = pow(finalColor, vec3(0.4545));

    


    // Output the final color with all effects applied
    color = vec4(finalColor, 1.0);
}