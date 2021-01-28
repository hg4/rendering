#version 330 core
out vec4 FragColor;

in vec3 WorldPos; 
in vec4 FragPosInLightSpace;
in vec3 Normal;  
in vec2 TexCoords;
in mat3 TtoW;
float PI = 3.14159265359;

struct PbrData{
	vec3 albedo;
	float roughness;
	float metallic;
	float ao;
	vec3 F0;
};

struct PointLight{
	vec3 position;
	vec3 color;
	vec3 attenuation;
	float intensity;
};
struct DirLight{
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
};

uniform bool enableShadow;
uniform sampler2D shadowMap;
uniform float bias;

uniform vec3 viewPos;

uniform PointLight pointLight[10];
uniform DirLight dirLight[10];
uniform int pointLightNum;
uniform int dirLightNum;

uniform sampler2D texture_albedo;
uniform sampler2D texture_ao;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
uniform sampler2D texture_normal;

uniform float sheen;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
//uniform float prefilterLodLevel;
uniform sampler2D brdfLUT;

float pow5(float a){
	return a*a*a*a*a;
}
vec3 F_Schlick(float HdotV,vec3 F0){
	return F0+(1.0-F0)*pow5(1.0-HdotV);
}


float ShadowCalculation(vec4 fragPosLightSpace,float NdotL)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow

	float Bias = max(0.05 * (1.0 - NdotL), 0.005);
    float shadow = currentDepth -Bias> closestDepth  ? 1.0 : 0.0;
	shadow = (projCoords.z > 1.0) ? 0.0:shadow;
 
    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 10;
    const float maxLayers = 20;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * 0.02; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = pow(texture(texture_metallic, currentTexCoords).r,2.2);
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = pow(texture(texture_metallic, currentTexCoords).r,2.2);  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // -- parallax occlusion mapping interpolation from here on
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = pow(texture(texture_metallic, prevTexCoords).r,2.2) - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float ClothNDF(float NdotH, float a)
{
	float k = NdotH>0 ? 1.0:0.0;
	float ra = 1.0/a;
	float nominator = k * (2 + ra) * pow(1-NdotH*NdotH,0.5*ra);
	float denominator =2*PI;
	return nominator/(denominator+0.0001);
}

float D_Ashikhmin(float roughness, float NoH) {
    // Ashikhmin 2007, "Distribution-based BRDFs"
	float a2 = roughness * roughness;
	float cos2h = NoH * NoH;
	float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
	float sin4h = sin2h * sin2h;
	float cot2 = -cos2h / (a2 * sin2h);
	return 1.0 / (PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
}

vec3 ClothBRDF(vec3 N,vec3 V, vec3 L,PbrData data)
{
	vec3 H = normalize(L+V);
	float HdotL = max(dot(H,L),0.0);
	float NdotL = max(dot(N,L),0.0);
	float NdotV = max(dot(N,V),0.0);
	float NdotH = max(dot(N,H),0.0);
	//vec3 F = F_Schlick(HdotL,data.F0);
	float luminance = dot(data.albedo,vec3(0.299,0.587,0.114));
    vec3 F = vec3(0.04);
	vec3 diffuse = (1-F)*data.albedo/PI;
	vec3 sss = ((NdotL+0.5)/2.25)*(data.albedo+NdotL);
	diffuse *= sss;
	float denominator = 4*(NdotL+NdotV-NdotL*NdotV);
	vec3 specular = NdotL*F*ClothNDF(NdotH,data.roughness)/(denominator+0.0001);
//	float shadow = enableShadow ? ShadowCalculation(FragPosInLightSpace,NdotL) : 0.0;
//	return shadow == 1.0 ? vec3(0.0) : diffuse+specular;
	return (diffuse+specular);
}


void main(){
	PbrData pbrData;
	vec3 Tnormal= normalize(2 * texture(texture_normal,TexCoords).rgb - 1);
	vec3 N=normalize(TtoW * Tnormal);
	mat3 i_TtoW = inverse(TtoW);
	vec3 V=normalize(viewPos-WorldPos);
	vec3 viewDirInTangent = normalize(i_TtoW * V);
	vec2 texCoords = ParallaxMapping(TexCoords,viewDirInTangent);
	//vec2 texCoords = TexCoords;
	vec3 F0=vec3(0.04);
	pbrData.albedo=pow(texture(texture_albedo,texCoords).rgb,vec3(2.2));
	//pbrData.albedo = vec3(0.85,0.85,0.85);
	//pbrData.metallic=pow(texture(texture_metallic,texCoords).r,2.2);
	pbrData.roughness=pow(texture(texture_roughness,texCoords).r,2.2);
	pbrData.ao=pow(texture(texture_ao,texCoords).r,2.2);
	pbrData.F0=vec3(0.04);
	vec3 result=vec3(0.0);
	float NdotV=max(dot(N,V),0.0);

	for(int i=0;i<pointLightNum;i++){
		vec3 L=normalize(pointLight[i].position-WorldPos);
		float distance=length(pointLight[i].position-WorldPos);
		float attenuation=1.0/(distance*distance);
		vec3 radiance=pointLight[i].color*attenuation;
		//result+=sumDistribution(N,V,L,pbrData,radiance,pointLight[i].intensity);
		result+=ClothBRDF(N,V,L,pbrData)*radiance*pointLight[i].intensity;
	}
	
	for(int i=0;i<dirLightNum;i++){
		vec3 L=vec3(0.0)-normalize(dirLight[i].direction);
		//float distance=length(dirLight[i].position-FragPos);
		float attenuation=1.0;
		vec3 radiance=dirLight[i].color*attenuation;
		//result+=sumDistribution(N,V,L,pbrData,radiance,dirLight[i].intensity);
		result+=ClothBRDF(N,V,L,pbrData)*radiance*dirLight[i].intensity;
	}
	vec3 R=reflect(-V,N);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 kS=F_Schlick(NdotV,F0);
	vec3 kD=1.0-kS;
//  vec3 prefilteredColor = textureLod(prefilterMap, R,  pbrData.roughness * MAX_REFLECTION_LOD).rgb; 
//	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), pbrData.roughness)).rg;
//	vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
	
	vec3 irradiance=texture(irradianceMap,N).rgb;
	result+=kD*irradiance*pbrData.albedo*PI;
	//result+=specular*0.8;
	result*=pbrData.ao;
	//float shadow = enableShadow ? ShadowCalculation(FragPosInLightSpace,NdotL) : 0.0;
	//result = shadow ==1.0 ? pbrData.albedo*0.05 : result;
	result=result/(result+vec3(1.0));
	result=pow(result,vec3(1.0/2.2));
	FragColor=vec4(result,1.0);
}