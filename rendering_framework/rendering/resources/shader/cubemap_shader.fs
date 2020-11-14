#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform samplerCube environmentMap;

void main(){
		vec3 color=textureLod(environmentMap,FragPos,1.2).rgb;
		color=color/(color+1.0);
		color=pow(color,vec3(1.0/2.2)); 
		//��hdrתΪldr����gammaУ�����͸����ȵĿ�������ʾ����Ϊֻ������ʾ������gammaУ�����ֵ����
		FragColor=vec4(color,1.0);
}