#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Shader.h"
using namespace std;
using namespace glm;
class Vertex{
public:
	Vertex(){}
	Vertex(vec3 _position, vec3 _normal, vec2 _texCoord):
		position(_position),normal(_normal),texCoords(_texCoord){}
	vec3 position;
	vec3 normal;
	vec2 texCoords;
};
class Texture{
public:
	Texture(){}
	Texture(unsigned int _id, string _type,string _path) :type(_type), id(_id),path(_path){}
	unsigned int id;
	string path;
	string type;
};
class Mesh{
public:
	Mesh(){}
	Mesh(vector<Vertex> &_v, vector<unsigned int> _i, vector<Texture> &_t){
		vertices = _v;
		indices = _i;
		textures = _t;
		init();
	}
	//Mesh(const string path);
	void draw(Shader &shader);
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

private:
	unsigned int VAO, VBO, EBO;
	void init();
	
};