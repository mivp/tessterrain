#version 410

in vec3 Position0;
in vec3 Normal0; 
in vec2 TexCoord0; 

layout( location = 0 ) out vec4 FragColor;

vec3 lightDir = vec3(0.5, 0.5, 1);
float material_kd = 0.8;
float material_ks = 0.2;
int material_shininess = 2;

void main() 
{ 
	vec3 L = normalize(lightDir); // light direction
	vec3 V = normalize( -1 * Position0); //view direction

	float LdotN = max(0, dot(L, Normal0));
	float diffuse = material_kd * LdotN;
	float specular = 0;

	if(LdotN > 0.0) {
	 
		//vec3 R = -normalize(reflect(L, Normal0));//Reflection
		//specular = material_ks * pow(max(0, dot(R, V)), material_shininess);
	 
		//Blinn-Phong
		vec3 H = normalize(L + V );//Halfway
		specular = material_ks * pow(max(0, dot(H, Normal0)), material_shininess);
	}

	float light = diffuse + specular;
    FragColor = vec4(vec3(light), 1);
}
