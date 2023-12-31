#version 330 core
uniform mat4 Transform;
layout (location = 0) in vec2 VertexPosition;
layout (location = 1) in vec2 VertexUV;
layout (location = 3) in vec4 VertexColor;
out vec4 Color;
out vec2 UV;
void main()
{
Color = VertexColor;
UV = VertexUV;
gl_Position = vec4(VertexPosition.x,VertexPosition.y,0.0,1.0) * Transform;
}