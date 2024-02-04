#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

layout (location = 0) out vec2 texCoord;

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
    texCoord = aTexCoord;
}