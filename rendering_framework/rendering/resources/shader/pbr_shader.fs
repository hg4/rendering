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

float D_GTR2(float NdotH,float r){
	float a=r*r;
	float a2=a*a;
	float cos2=NdotH*NdotH;
	float den=1.0+(a2-1.0)*cos2;
	return a2/max(PI*den*den,0.001);
}

float ShadowCalculation(vec4 fragPosLightSpace,float bias)
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

    float shadow = currentDepth -bias> closestDepth  ? 1.0 : 0.0;
	shadow = (projCoords.z > 1.0) ? 0.0:shadow;
 
    return shadow;
}

float smithG_GGX(float NdotV,float alpha){
		float k=(alpha+1)*(alpha+1)/8.0;
		return NdotV/(NdotV*(1.0-k)+k);
}
float smithG_GGX_disney(float NdotV,float roughness){
		float alpha=(0.5+roughness/2)*(0.5+roughness/2);
		float a=alpha*alpha;
		float b=NdotV*NdotV;
		return 2*NdotV/(NdotV+sqrt(a+b-a*b));
}
float GeometrySmith(vec3 N,vec3 V,vec3 L,float alpha){
		float NdotV=max(dot(N,V),0.0);
		float NdotL=max(dot(N,L),0.0);
	//	float ggx1=smithG_GGX(NdotV,alpha);
	//	float ggx2=smithG_GGX(NdotL,alpha);
		float ggx1=smithG_GGX_disney(NdotV,alpha);
		float ggx2=smithG_GGX_disney(NdotL,alpha);
		return ggx1*ggx2;
}
vec3 sumDistribution(vec3 N,vec3 V, vec3 L,PbrData pbr, vec3 radiance,float intensity){
	vec3 H=normalize(L+V);
	float NdotL=max(dot(N,L),0.0);
	float NdotH=max(dot(N,H),0.0);
	float HdotV=max(dot(H,V),0.0);
	float NdotV=max(dot(N,V),0.0);
	float NDF=D_GTR2(NdotH,pbr.roughness);
	float G=GeometrySmith(N,V,L,pbr.roughness);		
	vec3 F=F_Schlick(HdotV,pbr.F0);

	vec3 nominator=NDF*G*F;
	float denominator=4.0*NdotV*NdotL;
	vec3 specular=nominator/max(denominator,0.001);
	
	vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - pbr.metallic;	  
	float FD90=0.5+2*pbr.roughness*HdotV*HdotV;
	float a=1+(FD90-1)*pow5(1-NdotL);
	float b=1+(FD90-1)*pow5(1-NdotV);
	vec3 diffuse=pbr.albedo/PI*a*b;//disney BRDF diffuse
	float shadow = enableShadow ? ShadowCalculation(FragPosInLightSpace,0.05) : 0.0;
	return shadow == 1.0 ? vec3(0.0) : (kD*diffuse+specular) * radiance * NdotL * intensity;	
}


void main(){
	PbrData pbrData;
	pbrData.albedo=texture(texture_albedo,TexCoords).rgb;
	
	vec3 Tnormal= normalize(2 * texture(texture_normal,TexCoords).rgb - 1);
	vec3 N=normalize(TtoW * Tnormal);
	vec3 V=normalize(viewPos-WorldPos);
	vec3 F0=vec3(0.04);
	pbrData.metallic=texture(texture_metallic,TexCoords).r;
	pbrData.roughness=texture(texture_roughness,TexCoords).r;
	pbrData.ao=texture(texture_ao,TexCoords).r;
	pbrData.F0=mix(F0,pbrData.albedo,pbrData.metallic);
	vec3 result=vec3(0.0);
	float NdotV=max(dot(N,V),0.0);

	for(int i=0;i<pointLightNum;i++){
		vec3 L=normalize(pointLight[i].position-WorldPos);
		float distance=length(pointLight[i].position-WorldPos);
		float attenuation=1.0/(distance*distance);
		vec3 radiance=pointLight[i].color*attenuation;
		result+=sumDistribution(N,V,L,pbrData,radiance,pointLight[i].intensity);
	}
	
	for(int i=0;i<dirLightNum;i++){
		vec3 L=vec3(0.0)-normalize(dirLight[i].direction);
		//float distance=length(dirLight[i].position-FragPos);
		float attenuation=1.0;
		vec3 radiance=dirLight[i].color*attenuation;
		result+=sumDistribution(N,V,L,pbrData,radiance,dirLight[i].intensity);
	}
	vec3 R=reflect(-V,N);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 kS=F_Schlick(NdotV,F0);
	vec3 kD=1.0-kS;
	kD*=1-pbrData.metallic;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  pbrData.roughness * MAX_REFLECTION_LOD).rgb; 
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), pbrData.roughness)).rg;
	vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
	
	vec3 irradiance=texture(irradianceMap,N).rgb;
	result+=kD*irradiance*pbrData.albedo*PI;
	result+=specular;
	result*=pbrData.ao;
	float shadow = enableShadow ? ShadowCalculation(FragPosInLightSpace,bias) : 0.0;
	result = shadow ==1.0 ? pbrData.albedo*0.05 : result;
	result=result/(result+vec3(1.0));
	result=pow(result,vec3(1.0/2.2));
	FragColor=vec4(result,1.0);
}