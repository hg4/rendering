#include "Model.h"
#include <string>
#include "stb_image.h"
using namespace std;

Model::Model(shared_ptr<RenderObject> ro) {
	init();
	shared_ptr<RenderObject> temp(ro);
	_renderObjList.push_back(temp);
	renderObjDictionary[temp->name] = temp;
}

// draws the model, and thus all its meshes
//void Model::Draw(Shader &shader)
//{
//	for (unsigned int i = 0; i < meshes.size(); i++)
//		meshes[i]->draw(shader);
//}
//void Model::Draw()
//{
//	for (unsigned int i = 0; i < meshes.size(); i++)
//		meshes[i]->draw();
//}
//void Model::setMVP(const MVPTransform & mvp) {
//	for (unsigned int i = 0; i < meshes.size(); i++)
//		meshes[i]->setMVP(mvp);
//}
void Model::Render() {
	/*if (_updateMVP) {
		SetMVP(_parentModel);
	}*/
	for (int i = 0; i < _renderObjList.size(); i++) {
		_renderObjList[i]->Render();
	}
}

void Model::SetMaterial(Material * _mtr)
{
	mtr = shared_ptr<Material>(_mtr);
	for (int i = 0; i < _renderObjList.size(); i++)
		_renderObjList[i]->SetMaterial(_mtr);
}

void Model::SetParentScene(shared_ptr<Scene> s)
{
	parentScene = s;
	for (int i = 0; i < _renderObjList.size(); i++)
	{
		_renderObjList[i]->SetParentScene(s);
	}
}

void Model::SetMVP(const mat4 & parentModel)
{
	_parentModel = parentModel;
	for (int i = 0; i < _renderObjList.size(); i++) 
	{
		_renderObjList[i]->SetMVP(_parentModel * transform->GetModel());
		//GetModel() return transpose matrix,so reverse the mvp order.
	}
	_updateMVP = false;
}

shared_ptr<RenderObject> Model::GetRenderObjectByName(const string & objName)
{
	return renderObjDictionary[objName];
}

void Model::init()
{
	parentScene = nullptr;
	mtr = nullptr;
	_updateMVP = true;
}
void Model::ClearRenderObjectTextures(const string & objName)
{
	shared_ptr<RenderObject> ro = renderObjDictionary[objName];
	ro->mtr->textureList.clear();
}
//void Model::Scale(vec3 s)
//{
//	transform->SetLocalScale(s);
//	_updateMVP = true;
//}
//// 3 components mean xyz axis rotation by angle(not radians) 
//void Model::Rotate(vec3 r)
//{
//	transform->SetLocalRotate(r);
//	_updateMVP = true;
//}
//
//void Model::Translate(vec3 t)
//{
//	transform->SetTranslate(t);
//	_updateMVP = true;
//}


//unsigned int Model::TextureFromFile(const char *path, const string &directory, bool gamma)
//{
//	string filename = string(path);
//	filename = directory + '/' + filename;
//
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//
//	int width, height, nrComponents;
//	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//	if (data)
//	{
//		GLenum format;
//		if (nrComponents == 1)
//			format = GL_RED;
//		else if (nrComponents == 3)
//			format = GL_RGB;
//		else if (nrComponents == 4)
//			format = GL_RGBA;
//
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//		glGenerateMipmap(GL_TEXTURE_2D);
//
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//		stbi_image_free(data);
//	}
//	else
//	{
//		std::cout << "Texture failed to load at path: " << path << std::endl;
//		stbi_image_free(data);
//	}
//
//	return textureID;
//}
void Model::AddRenderObject(RenderObject * r)
{
	AddRenderObject(shared_ptr<RenderObject>(r));
}

void Model::AddRenderObject(shared_ptr<RenderObject> r)
{
	_renderObjList.push_back(r);
	r->parent = shared_ptr<RenderObject>(this);
}

void Model::loadModel(string const &path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode *node, const aiScene *scene)
{

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		shared_ptr<Mesh> ms = shared_ptr<Mesh>(processMesh(mesh, scene));
	//	meshes.push_back(ms);
		string ms_name = mesh->mName.data;
		if (ms_name == "") ms_name = "Default";
		shared_ptr<RenderObject> ro(new RenderObject(ms, ms_name));
		AddRenderObject(ro);
		renderObjDictionary[ms_name] = ro;
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}

Mesh* Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vector<Tangent> tangents;
	// walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		Tangent tan;
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
						  // positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		// normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}
		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
			// tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			tan.tangent = vector;
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			tan.bitangent = vector;
		}
		else
			vertex.texCoords = glm::vec2(0.0f, 0.0f);
		tangents.push_back(tan);
		vertices.push_back(vertex);
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	//aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	//// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	//// Same applies to other texture as the following list summarizes:
	//// diffuse: texture_diffuseN
	//// specular: texture_specularN
	//// normal: texture_normalN

	//// 1. diffuse maps
	//vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//// 2. specular maps
	//vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//// 3. normal maps
	//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	//textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	//// 4. height maps
	//std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
	//textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// return a mesh object created from the extracted mesh data 
	return new Mesh(vertices, indices, textures,tangents);
}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
//vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
//{
//	vector<Texture> textures;
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (unsigned int j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//			{
//				textures.push_back(textures_loaded[j]);
//				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip)
//		{   // if texture hasn't been loaded already, load it
//			Texture texture;
//			texture.id = TextureFromFile(str.C_Str(), this->directory);
//			texture.name_in_shader = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//		}
//	}
//	return textures;
//}
//	
