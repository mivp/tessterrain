#version 410

//IMG_SHADER_CODE_TEXTURE_LOOKUP

in vec2 OutTexCoord;

layout( location = 0 ) out vec4 FragColor;

uniform sampler2D tex0;

void main()
{
	vec3 c = texture(tex0, OutTexCoord).rgb;
	FragColor = vec4(c, 1.0);
}