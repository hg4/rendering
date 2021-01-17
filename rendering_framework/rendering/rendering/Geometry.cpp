#include "Geometry.h"

shared_ptr<Mesh> Geometry::createCube()
{
	float vertices[] = {
		// back face
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
		-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
		-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
															  // front face
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
		-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
		-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
															  // left face
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
		-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
		-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
															  // right face
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
															 // bottom face
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
		-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
		-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
															  // top face
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
		-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
		-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
	};
	// render Cube
	return shared_ptr<Mesh>(new Mesh(vertices, 8 * 6 * 6, 8));
}

shared_ptr<Mesh> Geometry::createQuad()
{
	//float quadVertices[] = {
	//	// positions        // texture Coords
	//	-1.0f,  1.0f, 0.0f,0.0f,0.0f,1.0f, 0.0f, 1.0f,
	//	-1.0f, -1.0f, 0.0f,0.0f,0.0f,1.0f, 0.0f, 0.0f,
	//	1.0f,  1.0f, 0.0f, 0.0f,0.0f,1.0f, 1.0f, 1.0f,
	//	1.0f, -1.0f, 0.0f, 0.0f,0.0f,1.0f, 1.0f, 0.0f,
	//};
	float quadVertices[] = {
		// positions        // texture Coords
		-1.0f, -1.0f, 0.0f,0.0f,0.0f,1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,0.0f,0.0f,1.0f, 0.0f, 1.0f,
		1.0f,  1.0f, 0.0f, 0.0f,0.0f,1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,0.0f,0.0f,1.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 0.0f,0.0f,1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 0.0f,0.0f,1.0f, 1.0f, 0.0f,
	};

	return shared_ptr<Mesh>(new Mesh(quadVertices, 8 * 6, 8, false, NULL, 0, GL_TRIANGLES));
}
shared_ptr<Mesh> Geometry::createPlane() {
	unsigned int planeVAO;
	float planeVertices[] = {
		// positions            // normals    //tangent bitangent     // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,   1.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f,   0.0f, 1.0f,

		//25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		//-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f, 1.0f,0.0f,0.0f, 0.0f,0.0f,1.0f, 1.0f, 1.0f
	};
	vector<Vertex> v;
	vector<Tangent> t;
	for (int i = 0; i < 6; i++) {
		vec3 position = vec3(planeVertices[i * 14 + 0], planeVertices[i * 14 + 1], planeVertices[i * 14 + 2]);
		vec3 normal = vec3(planeVertices[i * 14 + 3], planeVertices[i * 14 + 4], planeVertices[i * 14 + 5]);
		vec3 tangent = vec3(planeVertices[i * 14 + 6], planeVertices[i * 14 + 7], planeVertices[i * 14 + 8]);
		vec3 bitangent = vec3(planeVertices[i * 14 + 9], planeVertices[i * 14 + 10], planeVertices[i * 14 + 11]);
		vec2 uv = vec2(planeVertices[i * 14 + 12],planeVertices[i * 14 + 13]);
		v.push_back(Vertex(position, normal, uv));
		t.push_back(Tangent(tangent, bitangent));
	}
	vector<unsigned int> i = { 0,1,2,0,2,3 };
	// plane VAO
	/*unsigned int planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));*/

	return shared_ptr<Mesh>(new Mesh(v, i,vector<Texture>(),t));
	//return shared_ptr<Mesh>(new Mesh(planeVertices, 8 * 6, 8, false, nullptr, 0, GL_TRIANGLES));
}
shared_ptr<Mesh> Geometry::createSphere()
{
	//unsigned int sphereVAO = 0;
	unsigned int indexCount;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	const unsigned int X_SEGMENTS = 64;
	const unsigned int Y_SEGMENTS = 64;
	const float PI = 3.14159265359;
	for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
	{
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			float xSegment = (float)x / (float)X_SEGMENTS;
			float ySegment = (float)y / (float)Y_SEGMENTS;
			float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
			float yPos = std::cos(ySegment * PI);
			float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

			positions.push_back(glm::vec3(xPos, yPos, zPos));
			uv.push_back(glm::vec2(xSegment, ySegment));
			normals.push_back(glm::vec3(xPos, yPos, zPos));
		}
	}

	bool oddRow = false;
	for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
	{
		if (!oddRow) // even rows: y == 0, y == 2; and so on
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		else
		{
			for (int x = X_SEGMENTS; x >= 0; --x)
			{
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				indices.push_back(y       * (X_SEGMENTS + 1) + x);
			}
		}
		oddRow = !oddRow;
	}
	indexCount = indices.size();

	std::vector<float> data;
	for (unsigned int i = 0; i < positions.size(); ++i)
	{
		data.push_back(positions[i].x);
		data.push_back(positions[i].y);
		data.push_back(positions[i].z);
		if (normals.size() > 0)
		{
			data.push_back(normals[i].x);
			data.push_back(normals[i].y);
			data.push_back(normals[i].z);
		}
		if (uv.size() > 0)
		{
			data.push_back(uv[i].x);
			data.push_back(uv[i].y);
		}

	}
	Mesh m(data, 8, indices, GL_TRIANGLE_STRIP);
	//Mesh m(data.data(), data.size(), 8, true, indices.data(), indices.size(), GL_TRIANGLE_STRIP);
	return shared_ptr<Mesh>(&m);

}