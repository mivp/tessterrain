#version 410
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uMVP;
uniform mat4 uModelView;
uniform mat4 uNormalMatrix;

out vec3 Position0;
out vec3 Normal0;
out vec2 TexCoord0;

void main(void)
{
   gl_Position = uMVP * vec4(aPosition, 1);
   Position0 = (uModelView * vec4(aPosition, 1)).xyz;
   Normal0 = (uNormalMatrix * vec4(aNormal,0.0)).xyz;
   TexCoord0 = aTexCoord;
}
