#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Material.h"
#include "PbrMaterial.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Light.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "Camera.h"
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "MVPTransform.h"
using namespace std;

class Scene;
class RenderObject;


class Model
{
public:
	// model data 
	vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<shared_ptr<Mesh>> meshes;
	vector<shared_ptr<RenderObject>> renderObjList;
	string directory;
	map<string, shared_ptr<RenderObject>> renderObjDictionary;
	shared_ptr<Scene> parentScene;
	bool gammaCorrection;

	Model()
	{
		init();
	}
	// constructor, expects a filepath to a 3D model.
	Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
	{
		init();
		loadModel(path);
	}
	Model(shared_ptr<RenderObject> ro);
	/*void Draw(Shader & shader);
	void Draw();*/
	void Render();
	//void setMVP(const MVPTransform & mvp);
	void SetParentScene(shared_ptr<Scene> s);
	void ClearRenderObjectTextures(const string &objName);
	shared_ptr<RenderObject> GetRenderObjectByName(const string &objName);
	void Scale(vec3 s);
	void Rotate(vec3 r);
	void Translate(vec3 t);
	/*void ApplyLightForAll(shared_ptr<Light> l);
	void ApplyLightForObj(shared_ptr<Light> l, const string &objName);
	void ApplyCamera(shared_ptr<Camera> cam);*/
private:
	// loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string const & path);
	void processNode(aiNode * node, const aiScene * scene);
	Mesh * processMesh(aiMesh * mesh, const aiScene * scene);
	//vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName);
	//unsigned int TextureFromFile(const char * path, const string & directory, bool gamma = false);
	void init();
};
#endif
