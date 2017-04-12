#version 410

//IMG_VERTEX_CODE
layout(location = 0) in vec3 InVertex;
layout(location = 1) in vec3 InTexCoord;

out vec2 OutTexCoord;

void main ()
{
	gl_Position = vec4(InVertex, 1.0);
	OutTexCoord = InTexCoord.xy;
}