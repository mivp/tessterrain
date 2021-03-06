#version 400

#define M_PI 3.14159265358979
#define ESP 0.000001

subroutine vec4 ShaderModelType();
subroutine uniform ShaderModelType shaderModel;

uniform struct LineInfo {
  float width;
  vec4 color;
} line;

uniform struct FogInfo {
    bool enabled;
    vec4 color;
    float minDistance;
    float maxDistance;
} fog;

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

uniform sampler2D tex0; // texture
uniform sampler2D tex1; // overlay texture
uniform float overlayAlpha = 0.7;

uniform float colorStop1 = 0.0;
uniform float colorStop2 = 4.0;
uniform float colorStop3 = 8.0;
uniform float colorStop4 = 12.0;
uniform float colorStop5 = 16.0;

uniform vec4 color1 = vec4( 0.27, 0.00, 0.53, 1.00 );
uniform vec4 color2 = vec4( 0.11, 0.51, 0.73, 1.00 );
uniform vec4 color3 = vec4( 0.42, 0.76, 0.36, 1.00 );
uniform vec4 color4 = vec4( 0.92, 0.75, 0.34, 1.00 );
uniform vec4 color5 = vec4( 1.00, 1.00, 0.53, 1.00 );

in wireFrameVertex {
    noperspective vec3 edgeDistance;
    vec4 worldPosition;
    vec3 worldNormal;
    vec4 position;
    vec3 normal;
    vec2 texCoords;
};

uniform float opacity = 1.0;
uniform float heightCutOff = 0.0;

layout (location = 0) out vec4 fragColor;

// Helper functions
vec4 linearGradient( const in float t )
{
    vec4 color;
    if ( t < colorStop1 )
    {
        color = color1;
    }
    else if ( t < colorStop2 )
    {
        float tLocal = ( t - colorStop1 ) / ( colorStop2 - colorStop1 );
        color = mix( color1, color2, tLocal );
    }
    else if ( t < colorStop3 )
    {
        float tLocal = ( t - colorStop2 ) / ( colorStop3 - colorStop2 );
        color = mix( color2, color3, tLocal );
    }
    else if ( t < colorStop4 )
    {
        float tLocal = ( t - colorStop3 ) / ( colorStop4 - colorStop3 );
        color = mix( color3, color4, tLocal );
    }
    else if ( t < colorStop5 )
    {
        float tLocal = ( t - colorStop4 ) / ( colorStop5 - colorStop4 );
        color = mix( color4, color5, tLocal );
    }
    else
    {
        color = color4;
    }

    return color;
}

void phongModel( out vec3 ambientAndDiff, out vec3 spec )
{
    // Some useful vectors
    vec3 s = normalize( vec3( light.position )/* - position.xyz*/ );
    vec3 v = normalize( -position.xyz );
    vec3 n = normalize( normal );
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

vec4 wireFrame( const in vec4 color, const in vec4 wireFrameColor )
{
    // Find the smallest distance between the fragment and a triangle edge
    float d = min( edgeDistance.x, edgeDistance.y );
    d = min( d, edgeDistance.z );

    // Blend between line color and triangle color
    float mixVal;
    if ( d < line.width - 0.5 )
    {
        mixVal = 1.0;
    }
    else if ( d > line.width + 0.5 )
    {
        mixVal = 0.0;
    }
    else
    {
        float x = d - ( line.width - 1.0 );
        mixVal = exp2( -2.0 * ( x * x ) );
    }
    
    vec4 c = mix( color, wireFrameColor, mixVal );

    return c;
}

vec4 HSVtoRGB(float h, float s, float v) 
{
  float r, g, b, f, p, q, t;
  int i = int(floor(h * 6));
  f = h * 6 - i;
  p = v * (1 - s);
  q = v * (1 - f * s);
  t = v * (1 - (1 - f) * s);
  switch (i % 6) {
      case 0: r = v, g = t, b = p; break;
      case 1: r = q, g = v, b = p; break;
      case 2: r = p, g = v, b = t; break;
      case 3: r = p, g = q, b = v; break;
      case 4: r = t, g = p, b = v; break;
      case 5: r = v, g = p, b = q; break;
  }
  return vec4(r, g, b, 1.0);
}

// ShaderModelType Subroutines
subroutine( ShaderModelType )
vec4 shadeSimpleWireFrame()
{
    vec4 color = vec4( 1.0 );
    return wireFrame( color, line.color );
}

subroutine( ShaderModelType )
vec4 shadeWorldHeight()
{
    vec4 color = linearGradient( worldPosition.y );
    return color;
}

subroutine( ShaderModelType )
vec4 shadeWorldHeightWireFrame()
{
    vec4 color = linearGradient( worldPosition.y );
    return wireFrame( color, 0.7 * color );
}

subroutine( ShaderModelType )
vec4 shadeWorldNormal()
{
    return vec4( 0.5 * ( worldNormal + vec3( 1.0 ) ), 1.0 );
}

subroutine( ShaderModelType )
vec4 shadeLightingFactors()
{
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec );
    vec4 lightingFactors = vec4( ambientAndDiff, 1.0 ) + vec4( spec, 1.0 );
    return lightingFactors;
}

subroutine( ShaderModelType )
vec4 shadeTextured()
{
    return texture (tex0, texCoords);
}

subroutine( ShaderModelType )
vec4 shadeTexturedAndLit()
{
    // Get texture color
    vec4 texColor = texture (tex0, texCoords);
    
    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec );
    vec4 color = vec4( ambientAndDiff, 1.0 ) * texColor + vec4( spec, 1.0 );
    return color;
}

subroutine( ShaderModelType )
vec4 shadeTexturedAndOverlay()
{
    vec4 c0 = texture (tex0, texCoords);
    vec4 c1 = texture (tex1, texCoords);
    
    if(c1.a < 0.1)
        return c0;

    if(c1.a > 0.9)
        return c1;

    return vec4(mix(c0.rgb, c1.rgb, overlayAlpha), 1.0);
}

subroutine( ShaderModelType )
vec4 shadeTexturedAndOverlayAndLit()
{
    vec4 c0 = texture (tex0, texCoords);
    vec4 c1 = texture (tex1, texCoords);

    vec4 texColor;    

    if(c1.a < 0.1)
        texColor = c0;

    else if(c1.a > 0.9)
        texColor = c1;

    else
    	texColor = vec4(mix(c0.rgb, c1.rgb, overlayAlpha), 1.0);

    // Calculate the lighting model, keeping the specular component separate
    vec3 ambientAndDiff, spec;
    phongModel( ambientAndDiff, spec );
    vec4 color = vec4( ambientAndDiff, 1.0 ) * texColor + vec4( spec, 1.0 );
    return color;
}

subroutine( ShaderModelType )
vec4 shadeTrendPlunge()
{
    vec3 n = normalize(worldNormal);
    if(n.y < 0) n = -1*n;

    float trend;
    trend = atan(n.x, n.z+1E-18) / (2*M_PI);
    if(trend < 0) trend += 1;

    float plunge = 1 - n.y;
    plunge = max(plunge, 0.25);

    vec4 color = HSVtoRGB(trend, plunge, 1.0); 
    //vec4 color = linearGradient(1.25*colorStop5*plunge);
    return wireFrame( color, 0.7 * color );
}

void main()
{
    if(worldPosition.y < heightCutOff)
        discard;

    // Compute fragment color depending upon selected shading mode
    vec4 c = shaderModel();

    if(fog.enabled) 
    {
        // Blend with fog color
        float dist = abs( position.z );
        float fogFactor = ( fog.maxDistance - dist ) / ( fog.maxDistance - fog.minDistance );
        fogFactor = clamp( fogFactor, 0.3, 1.0 );
        fragColor = mix( fog.color, c, fogFactor );
    }
    else 
    {
        fragColor = c;
    }
    fragColor.a = opacity;
}
