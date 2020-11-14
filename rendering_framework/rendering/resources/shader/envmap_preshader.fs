#version 330 core
in vec3 FragPos;
out vec4 FragColor;

uniform samplerCube environmentMap;

const float PI=3.14159265359;

vec3 sampleHalfSphere(vec3 normal){
	vec3 Lo=vec3(0.0);
	float sampleStep=0.01;
	int sampleCnt=0;
	vec3 up=vec3(0.0,1.0,0.0);
	vec3 right=cross(up,normal);
	up=cross(normal,right);
	vec3 front=normal;
	for(float phi=0.0;phi<2*PI;phi+=sampleStep){
		for(float theta=0.0;theta<PI/2;theta+=sampleStep){
			vec3 sample=vec3(sin(theta)*cos(phi),sin(theta)*sin(phi),cos(theta));
			vec3 world_dir=sample.x*right+sample.z*front+sample.y*up;
			vec3 Li=texture(environmentMap,world_dir).rgb;
			Lo+=Li*cos(theta)*sin(theta);
			sampleCnt++;
		}
	}
	Lo/=sampleCnt;
	return Lo;
}
void main(){
	vec3 normal=normalize(FragPos);
	vec3 irradiance=sampleHalfSphere(normal);
	FragColor=vec4(irradiance,1.0);
}