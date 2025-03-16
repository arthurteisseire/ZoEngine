//#version 460 core
//layout (location = 0) in vec4 texColor;
//layout (location = 1) in vec2 texCoord;
//
//layout (location = 0) out vec4 FragColor;
//layout (binding = 0) uniform sampler2D Texture;
//
//void main()
//{
//    FragColor = texture(Texture, texCoord) * texColor;
//}
//

#version 460 core

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

layout (binding = 0) uniform sampler2D uTexture;

// Lighting uniforms
//uniform vec3 uLightPos;   // Position of the light source in world space
//uniform vec3 uViewPos;    // Camera position in world space
//uniform vec3 uLightColor; // Color (and intensity) of the light
//uniform vec3 uObjectColor; // Base color of the object

layout (location = 0) out vec4 FragColor;

void main()
{
    // Ambient lighting
    float ambientStrength = 0.2;
    vec3 ambient = vec3(ambientStrength);// * uLightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(uLightPos - FragPos);
    vec3 lightDir = normalize(FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * uLightColor;
    vec3 diffuse = vec3(diff);

    // Specular lighting (using Phong model)
    float specularStrength = 0.5;
//    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 viewDir = normalize(FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
//    vec3 specular = specularStrength * spec * uLightColor;
    vec3 specular = vec3(specularStrength * spec);

    // Combine lighting components
//    vec3 lighting = (ambient + diffuse + specular) * uObjectColor;
    vec3 lighting = (ambient + diffuse + specular);

    // Sample the texture
    vec4 texColor = texture(uTexture, TexCoord);

    // Final fragment color
    FragColor = vec4(lighting, 1.0) * texColor;
}
