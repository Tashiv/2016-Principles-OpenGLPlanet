//// Declaration Guards
#ifndef GEOMETRY_H
#define GEOMETRY_H

//// Imports
#include <GLM/vec3.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

namespace SWPTAS001
{
	//// Enumerations
	enum OBJDataType{ NONE, VERTEX, TEXTURECOORD, NORMAL, FACE, COMMENT};

	//// Structures
	struct FaceData
	{
		int vertexIndex[3];
		int texCoordIndex[3];
		int normalIndex[3];
	};

	//// Classes
	class GeometryData
	{
		public:
			//// Loaders
			void loadFromOBJFile(std::string filename);
			//// Counters
			int vertexCount();
			int normalCount();
			int textureCoordCount();
			int tangentCount();
			int bitangentCount();
			//// Accessors
			void* vertexData();
			void* textureCoordData();
			void* normalData();
			void* tangentData();
			void* bitangentData();
			//// Utilities
			glm::vec3 findMaxDimensions();

		private:
			//// Object Data
			std::vector<float> vertices;
			std::vector<float> textureCoords;
			std::vector<float> normals;
			std::vector<float> tangents;
			std::vector<float> bitangents;
			//// Object Indexing
			std::vector<FaceData> faces;
	};
}

#endif

// NOTE: The WaveFront OBJ format spec, states that meshes are allowed to be defined by faces
//       consisting of 3 or more vertices. For the purposes of this loader (and since this is the
//       most common case) we only support triangle faces (defined by exactly 3 vertices) and as a
//       result, the loader will ignore any data after the third vertex for each face
//
//       Similarly, the spec allows for vertex positions and texture coordinates to both have a
//       w-coordinate. The loader will ignore these and assumes that all vertex specifications contain
//       exactly 3 values, and that all texture coordinate specifications contain exactly 2 values

// NOTE: There is currently no support for mtl material references or anything like that,
//       just load whatever texture you want to use manually