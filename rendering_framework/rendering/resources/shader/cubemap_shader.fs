#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform samplerCube environmentMap;

void main(){
		vec3 color=textureLod(environmentMap,FragPos,1.2).rgb;
		color=color/(color+1.0);
		color=pow(color,vec3(1.0/2.2)); 
		//将hdr转为ldr，并gamma校正降低高亮度的开销再显示，因为只用于显示所以用gamma校正后的值即可
		FragColor=vec4(color,1.0);
}