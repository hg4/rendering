#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

float PI = 3.14159265359;


uniform vec3 viewPos;
//uniform float metallic;
//uniform float roughness;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform sampler2D texture_albedo;
uniform sampler2D texture_ao;
uniform sampler2D texture_roughness;
uniform sampler2D texture_metallic;
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

void main(){
	vec3 albedo=texture(texture_albedo,TexCoords).rgb;
	//vec3 albedo=vec3(0.5,0.0,0.0);
	vec3 N=normalize(Normal);
	vec3 V=normalize(viewPos-FragPos);
	vec3 F0=vec3(0.04);
	float metallic=texture(texture_metallic,TexCoords).r;
	float roughness=texture(texture_roughness,TexCoords).r;
	float ao=texture(texture_ao,TexCoords).r;
	F0=mix(F0,albedo,metallic);
	vec3 result=vec3(0.0);
	float NdotV=max(dot(N,V),0.0);
/*	for(int i=0;i<4;i++){
		vec3 L=normalize(lightPositions[i]-FragPos);
		vec3 H=normalize(L+V);
		float distance=length(lightPositions[i]-FragPos);
		float attenuation=1.0/(distance*distance);
		vec3 radiance=lightColors[i]*attenuation;
		float NdotL=max(dot(N,L),0.0);
		float NdotH=max(dot(N,H),0.0);
		float HdotV=max(dot(H,V),0.0);
	
		float NDF=D_GTR2(NdotH,roughness);
		float G=GeometrySmith(N,V,L,roughness);		
		vec3 F=F_Schlick(HdotV,F0);

		vec3 nominator=NDF*G*F;
		float denominator=4.0*NdotV*NdotL;
		vec3 specular=nominator/max(denominator,0.001);
		
		 vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  
		float FD90=0.5+2*roughness*HdotV*HdotV;
		float a=1+(FD90-1)*pow5(1-NdotL);
		float b=1+(FD90-1)*pow5(1-NdotV);
		vec3 diffuse=albedo/PI*a*b;//disney BRDF diffuse
		result+=(kD*diffuse+specular)*radiance	*NdotL;
	}*/
	vec3 R=reflect(-V,N);
	const float MAX_REFLECTION_LOD = 4.0;
	vec3 kS=F_Schlick(NdotV,F0);
	vec3 kD=1.0-kS;
	kD*=1-metallic;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb; 
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (kS * envBRDF.x + envBRDF.y);
	
	vec3 irradiance=texture(irradianceMap,N).rgb;
	result+=kD*irradiance*albedo*PI;
	result+=specular;
	result*=ao;
	result=result/(result+vec3(1.0));
	result=pow(result,vec3(1.0/2.2));
	FragColor=vec4(result,1.0);
}