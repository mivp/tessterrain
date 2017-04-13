#version 400
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;

uniform mat4 uMVP;
uniform mat4 uModelView;
uniform mat3 uNormalMatrix;

out vec3 Position0;
out vec3 Normal0;
out vec2 TexCoord0;

void main(void)
{
   gl_Position = uMVP * vec4(inPosition, 1);
   Position0 = (uModelView * vec4(inPosition, 1)).xyz;
   Normal0 = uNormalMatrix * inNormal;
   TexCoord0 = inTexCoord;
}
