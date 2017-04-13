#version 400

in vec3 Position0;
in vec3 Normal0; 
in vec2 TexCoord0; 

layout( location = 0 ) out vec4 FragColor;

uniform struct LightInfo
{
    vec4 position;  // Light position in eye coords.
    vec3 intensity;
} light;

uniform struct MaterialInfo
{
    vec3 Ka;            // Ambient reflectivity
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ks;            // Specular reflectivity
    float shininess;    // Specular shininess exponent
} material;

void phongModel( out vec3 ambientAndDiff, out vec3 spec )
{
    // Some useful vectors
    vec3 s = normalize( vec3( light.position )/* - Position0.xyz*/ );
    vec3 v = normalize( -Position0.xyz );
    vec3 n = normalize( Normal0 );
    vec3 r = reflect( -s, n );

    // Calculate the ambient contribution
    vec3 ambient = light.intensity * material.Ka;

    // Calculate the diffuse contribution
    float sDotN = max( dot( s, n ), 0.0 );
    vec3 diffuse = light.intensity * material.Kd * sDotN;

    // Sum the ambient and diffuse contributions
    ambientAndDiff = ambient + diffuse;

    // Calculate the specular highlight component
    spec = vec3( 0.0 );
    if ( sDotN > 0.0 )
    {
        spec = light.intensity * material.Ks *
               pow( max( dot( r, v ), 0.0 ), material.shininess );
    }
}

void main() 
{ 
	vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

	// Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec );
    FragColor = vec4( ambientAndDiff, 1.0 ) * color + vec4( spec, 1.0 );
}
