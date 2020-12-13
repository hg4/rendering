#version 330 core

in vec2 TexCoords;
out vec3 FragColor;

const float PI=3.14159265359;
float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

//ͬ���ǲ������������ڲ��������ֲ���ͬ�����ǰ���ƽ������ƫ���ڲ���
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;

	//���ݵͲ����������������������������ģ����Ҫ�Բ���
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

float pow5(float a){
	return a*a*a*a*a;
}

float D_GTR2(float NdotH,float r){
	float a=r*r;
	float a2=a*a;
	float cos2=NdotH*NdotH;
	float den=1.0+(a2-1.0)*cos2;
	return a2/max(PI*den*den,0.001);
}

float smithG_GGX(float NdotV,float alpha){
		float k=alpha*alpha/2.0;
		return NdotV/(NdotV*(1.0-k)+k);
}

float GeometrySmith(vec3 N,vec3 V,vec3 L,float alpha){
		float NdotV=max(dot(N,V),0.0);
		float NdotL=max(dot(N,L),0.0);
		float ggx1=smithG_GGX(NdotV,alpha);
		float ggx2=smithG_GGX(NdotL,alpha);
	//	float ggx1=smithG_GGX_disney(NdotV,alpha);
	//	float ggx2=smithG_GGX_disney(NdotL,alpha);
		return ggx1*ggx2;
}

vec2 intergrateBRDF(float NdotV,float roughness){
	vec3 N=vec3(0.0,0.0,1.0);
	vec3 V;
	V.x=sqrt(1-NdotV*NdotV);
	V.y=0.0;
	V.z=NdotV;
	float A=0.0;
	float B=0.0;
	uint SAMPLE_COUNT=1024u;
	for(uint i=0u;i<SAMPLE_COUNT;i++){
		vec2 Xi=Hammersley(i,SAMPLE_COUNT);
		vec3 H=ImportanceSampleGGX(Xi,N,roughness);
		vec3 L=normalize(2.0*dot(V,H)*H-V);
		float NdotH=max(dot(N,H),0.0);
		float NdotL=max(dot(N,L),0.0);
		float VdotH=max(dot(V,H),0.0);
		if(NdotL>0.0){
			float G=GeometrySmith(N,V,L,roughness);
			/*����ʹ������Ҫ�Բ�������LUT����Ҫ����pdf
			��֪pdf=D*NdotH/(4*VdotH)  ��ʱδ֪��ô����
			f=DGF/(4*NdotV*NdotL)
			f/pdf=GF*VdotH/(NdotV*NdotL*NdotH)
			f*NdotL/pdf=GF*VdotH/��NdotH*NdotV)
			����F����ȡ�����㣬��G_vis=G*VdotH/(NdotH*NdotV)
			*/
			float G_vis=G*VdotH/(NdotH*NdotV);
			//float NDF=D_GTR2(NdotH,roughness);
			float a=pow5(1-VdotH);
			A+=(1.0-a)*G_vis;
			B+=a*G_vis;
		}
	}
	A/=float(SAMPLE_COUNT);
	B/=float(SAMPLE_COUNT);
	return vec2(A,B);
}
void main(){
	vec2 brdf=intergrateBRDF(TexCoords.x,TexCoords.y);
	FragColor=vec3(brdf,0.0);
}