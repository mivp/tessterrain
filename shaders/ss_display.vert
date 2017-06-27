#version 410

//IMG_VERTEX_CODE
layout(location = 0) in vec2 InVertex;

void main ()
{
	gl_Position = vec4(InVertex, 0.0, 1.0);
}