#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec4 texColor;
layout (location = 1) out vec2 texCoord;

// matrix layout:
// right-handed
//   i j k
// x
// y
// z

vec3 rotateOnZAxis(vec3 pos, float angle)
{
    float rad = radians(angle);
    mat3 rotationMatrix = mat3(
        cos(rad), -sin(rad), 0.0,
        sin(rad), cos(rad), 0.0,
        0.0, 0.0, 1.0
    );
    return rotationMatrix * pos;
}

void main()
{
    gl_Position = vec4(rotateOnZAxis(aPos, 0), 1.0);
    texColor = vec4(aColor, 1.0);
    texCoord = aTexCoord;
}