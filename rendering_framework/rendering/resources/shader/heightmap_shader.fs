#version 330 core
out vec4 FragColor;
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec4 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(texture_diffuse1, TexCoords).rgb;

    // diffuse 
    vec3 norm = normalize(Normal.xyz);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;  
 
	//specular
	vec3 reflect=reflect(-lightDir,norm);
	vec3 viewDir=normalize(viewPos-FragPos);
    float spec=pow(max(dot(reflect,viewDir),0),32);
	vec3 specular=light.specular*spec*texture(texture_specular1,TexCoords).rgb;
    vec3 result = ambient + diffuse+specular;
    FragColor = vec4(result, 1.0);
} 