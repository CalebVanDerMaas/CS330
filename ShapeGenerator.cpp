#include "ShapeGenerator.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Vertex.h"
#include <glm\vec4.hpp>


#define PI 3.14159265359
using glm::vec3;
using glm::mat4;
using glm::mat3;
using glm::vec4;
#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)

glm::vec3 randomColor()
{
	glm::vec3 ret;
	ret.x = rand() / (float)RAND_MAX;
	ret.y = rand() / (float)RAND_MAX;
	ret.z = rand() / (float)RAND_MAX;
	return ret;
}


ShapeData ShapeGenerator::makePlaneVerts(uint dimensions)
{
	ShapeData ret;
	ret.numVertices = dimensions * dimensions;
	int half = dimensions / 2;
	ret.vertices = new Vertex[ret.numVertices];
	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			Vertex& thisVert = ret.vertices[i * dimensions + j];
			thisVert.position.x = j - half;
			thisVert.position.z = i - half;
			thisVert.position.y = 0;
			thisVert.normal = glm::vec3(0.0f, 1.0f, 0.0f);
			thisVert.color = randomColor();
		}
	}
	return ret;
}

ShapeData ShapeGenerator::makePlaneIndices(uint dimensions)
{
	ShapeData ret;
	ret.numIndices = (dimensions - 1) * (dimensions - 1) * 2 * 3; // 2 triangles per square, 3 indices per triangle
	ret.indices = new unsigned short[ret.numIndices];
	int runner = 0;
	for (int row = 0; row < dimensions - 1; row++)
	{
		for (int col = 0; col < dimensions - 1; col++)
		{
			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;

			ret.indices[runner++] = dimensions * row + col;
			ret.indices[runner++] = dimensions * row + col + dimensions + 1;
			ret.indices[runner++] = dimensions * row + col + 1;
		}
	}
	assert(runner = ret.numIndices);
	return ret;
}


ShapeData ShapeGenerator::makePlane(uint dimensions)
{
	ShapeData ret = makePlaneVerts(dimensions);
	ShapeData ret2 = makePlaneIndices(dimensions);
	ret.numIndices = ret2.numIndices;
	ret.indices = ret2.indices;
	return ret;
}

ShapeData ShapeGenerator::makeSphere(uint tesselation)
{
	ShapeData ret = makePlaneVerts(tesselation);
	ShapeData ret2 = makePlaneIndices(tesselation);
	ret.indices = ret2.indices;
	ret.numIndices = ret2.numIndices;

	uint dimensions = tesselation;
	const float RADIUS = 1.0f;
	const double CIRCLE = PI * 2;
	const double SLICE_ANGLE = CIRCLE / (dimensions - 1);
	for (size_t col = 0; col < dimensions; col++)
	{
		double phi = -SLICE_ANGLE * col;
		for (size_t row = 0; row < dimensions; row++)
		{
			double theta = -(SLICE_ANGLE / 2.0) * row;
			size_t vertIndex = col * dimensions + row;
			Vertex& v = ret.vertices[vertIndex];
			v.position.x = RADIUS * cos(phi) * sin(theta);
			v.position.y = RADIUS * sin(phi) * sin(theta);
			v.position.z = RADIUS * cos(theta);
			v.normal = glm::normalize(v.position);
		}
	}
	return ret;
}
ShapeData ShapeGenerator::makeCylinder(uint slices, float radius, float height)
{
    ShapeData ret;
    const float RING_ANGLE = 2.0f * PI / slices;

    // Create the vertices
    ret.numVertices = slices * 2;
    ret.vertices = new Vertex[ret.numVertices];
    for (uint i = 0; i < slices; i++)
    {
        float x = radius * cos(i * RING_ANGLE);
        float z = radius * sin(i * RING_ANGLE);

        // Top vertex
        ret.vertices[i * 2].position = glm::vec3(x, height / 2.0f, z);
        ret.vertices[i * 2].normal = glm::normalize(glm::vec3(x, 0.0f, z));
        ret.vertices[i * 2].color = randomColor();

        // Bottom vertex
        ret.vertices[i * 2 + 1].position = glm::vec3(x, -height / 2.0f, z);
        ret.vertices[i * 2 + 1].normal = glm::normalize(glm::vec3(x, 0.0f, z));
        ret.vertices[i * 2 + 1].color = randomColor();
    }

    // Create the indices
    ret.numIndices = slices * 6;
    ret.indices = new unsigned short[ret.numIndices];
    for (uint i = 0, j = 0; i < slices; i++, j += 6)
    {
        uint topIndex = i * 2;
        uint bottomIndex = i * 2 + 1;
        uint nextTopIndex = (i + 1) % slices * 2;
        uint nextBottomIndex = (i + 1) % slices * 2 + 1;

        // Top triangle
        ret.indices[j] = topIndex;
        ret.indices[j + 1] = nextTopIndex;
        ret.indices[j + 2] = bottomIndex;

        // Bottom triangle
        ret.indices[j + 3] = bottomIndex;
        ret.indices[j + 4] = nextTopIndex;
        ret.indices[j + 5] = nextBottomIndex;
    }

    return ret;
}

ShapeData ShapeGenerator::makeHalfSphere(uint tesselation)
{
	ShapeData ret = makeSphere(tesselation);

	for (int i = 0; i < ret.numVertices; i++) {
		if (ret.vertices[i].position.z < 0) {
			ret.vertices[i].position.z = -ret.vertices[i].position.z;
			ret.vertices[i].normal.z = -ret.vertices[i].normal.z;
		}
	}

	return ret;
}

ShapeData ShapeGenerator::makeCube(float width, float height, float depth)
{
	ShapeData ret;
	const int numVertices = 8;
	const int numIndices = 36;
	Vertex vertices[numVertices] = {
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, // 0
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, // 1
		{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, // 2
		{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f)}, // 3
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},  // 4
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},  // 5
		{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},  // 6
		{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)}   // 7
	};
	GLushort indices[numIndices] = {
		0, 1, 2, 2, 3, 0,  // front face
		4, 6, 5, 4, 7, 6,  // back face
		1, 5, 6, 1, 6, 2,  // top face
		0, 3, 7, 0, 7, 4,  // bottom face
		3, 2, 6, 3, 6, 7,  // right face
		0, 4, 5, 0, 5, 1   // left face
	};
		ret.numVertices = numVertices;
		ret.numIndices = numIndices;
		ret.vertices = new Vertex[numVertices];
		ret.indices = new GLushort[numIndices];
		for (int i = 0; i < numVertices; i++) {
			ret.vertices[i] = vertices[i];
		}
		for (int i = 0; i < numIndices; i++) {
			ret.indices[i] = indices[i];
		}
		return ret;

	
}
	
