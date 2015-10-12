#version 400 core
layout (location = 0) in vec3 vertex; // <vec2 position, vec2 texCoords>
layout (location = 1) in vec2 texCoords; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = texCoords;
    //gl_Position = projection * view * vec4(vertex, 1.0);
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
}
