#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Transform.h"
using namespace std;
using namespace glm;

class MVPTransform {
public:
	mat4 model, view, projection,compose;
	mat4 transpose_inverse_model;
	MVPTransform(){
		model = mat4(1.0);
		view = mat4(1.0);
		projection = mat4(1.0);
		compose = mat4(1.0);
		//_basicTransform.clear();
	}
	MVPTransform(mat4 m,mat4 v,mat4 p):model(m),view(v),projection(p){
		compose = model*view*projection;
		transpose_inverse_model = transpose(inverse(model));
	}
	
private:

};