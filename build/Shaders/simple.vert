#version 330 core

in vec3 position;

uniform mat4 transformMVP;

void main()
{
    gl_Position = transformMVP * vec4(position,1.0f);
}
