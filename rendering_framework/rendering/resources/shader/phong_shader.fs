#version 330 core
out vec4 FragColor;
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight{
	vec3 position;
	vec3 direction;
	vec3 color;
	float intensity;
};

in vec3 WorldPos;  
in vec3 Normal;  
in vec2 TexCoords;
in mat3 TtoW;
uniform vec3 viewPos;
uniform sampler2D texture_albedo;
uniform sampler2D texture_normal;
//uniform sampler2D texture_specular1;
uniform DirLight dirLight[10];

void main()
{
    // ambient
    vec3 ambient = 0.05 * texture(texture_albedo, TexCoords).rgb;
	vec3 Tnormal= normalize(2 * texture(texture_normal,TexCoords).rgb - 1);
	vec3 norm=normalize(TtoW * Tnormal);
    // diffuse 
    // vec3 norm = normalize(Normal.xyz);
    vec3 lightDir = normalize(dirLight[0].position - WorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse =  diff * texture(texture_albedo, TexCoords).rgb;  
 
	//specular
	vec3 reflect=reflect(-lightDir,norm);
	vec3 viewDir=normalize(dirLight[0].direction);
    float spec=pow(max(dot(reflect,viewDir),0),32);
	vec3 specular=dirLight[0].color*spec*texture(texture_albedo,TexCoords).rgb;
    vec3 result = ambient + diffuse +specular;
    FragColor = vec4(result, 1.0);
} 