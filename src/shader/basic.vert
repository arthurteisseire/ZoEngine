//#version 460 core
//layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTexCoord;
//
//layout (location = 0) out vec4 texColor;
//layout (location = 1) out vec2 texCoord;
//
//layout (set = 1, binding = 0) uniform Matrices {
//    mat4 view;
//    mat4 projection;
//};
//
//// matrix layout:
//// right-handed
////   i j k
//// x
//// y
//// z
//
//vec3 rotateOnZAxis(vec3 pos, float angle)
//{
//    float rad = radians(angle);
//    mat3 rotationMatrix = mat3(
//        cos(rad), -sin(rad), 0.0,
//        sin(rad), cos(rad), 0.0,
//        0.0, 0.0, 1.0
//    );
//    return rotationMatrix * pos;
//}
//
//void main()
//{
//    gl_Position = projection * view * vec4(rotateOnZAxis(aPos, 0), 1.0);
//    texColor = vec4(aColor, 1.0);
//    texCoord = aTexCoord;
//}
//
//
#version 460 core

// Input vertex attributes
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

// Uniform matrices for transformation
layout (set = 1, binding = 0) uniform Matrices {
    mat4 uView;
    mat4 uProjection;
};

// Outputs to the fragment shader
layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TexCoord;

void main()
{
    // Transform vertex position to world space
//    vec4 worldPosition = uModel * vec4(aPosition, 1.0);
    vec4 worldPosition = vec4(aPosition, 1.0);
    FragPos = worldPosition.xyz;

    // Transform the normal using the inverse transpose of the model matrix
//    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    Normal = aNormal;

    // Pass through the texture coordinate
    TexCoord = aTexCoord;

    // Final vertex position in clip space
    gl_Position = uProjection * uView * worldPosition;
}
