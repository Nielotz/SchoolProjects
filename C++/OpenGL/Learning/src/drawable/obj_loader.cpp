#include "../headers/drawable/obj_loader.h"
#include "../headers/mygl/debug/debug.h"

#include <sstream>
#include <fstream>

namespace obj_loader
{
	std::vector<Point3D> Shape3DFromOBJ::getPoints() const
	{
		throw;
	}

	std::vector<Vertice3D> Shape3DFromOBJ::getVertices() const
	{
		return this->vertices;
	}

	std::pair<std::vector<Vertice3D>, std::vector<Point3D>> Shape3DFromOBJ::getVerticesWithNormals() const
	{
		return { this->vertices , this->normals };
	}

	std::shared_ptr<Shape3DFromOBJ> loadFromFile(const std::string& path)
	{
		std::ifstream objFile(path);
		ASSERT(objFile.is_open());

		std::shared_ptr<Shape3DFromOBJ> shape = std::make_shared<Shape3DFromOBJ>();

		std::vector<Point3D> points;
		std::vector<Point2D> textureCoordinates;
		std::vector<Point3D> normals;

		const std::string verticePositionsOption = "v";
		const std::string textureCoordinatesOption = "vt";
		const std::string normalsOption = "vn";
		const std::string verticeDataOption = "f";  // Indices to vertices, textures, normals. 

		const auto& extractFloats = [](const std::string& str) -> std::vector<float> {
			std::vector<float> floats;

			std::istringstream iss(str);

			// Iterate over the istream, using >> to grab floats and push_back to store them in the vector
			std::copy(std::istream_iterator<float>(iss),
				std::istream_iterator<float>(),
				std::back_inserter(floats));

			return floats;
		};

		const auto& getIndicesFromIndicesString = [](const std::string& str) -> std::vector<int> {
			std::vector<int> indices;
			std::stringstream ss(str);
			int indice;
			char delimeter;

			ss >> indice;
			indices.push_back(indice);

			ss >> delimeter;  // Skip "/".
			ss >> indice;
			indices.push_back(indice);

			ss >> delimeter;  // Skip "/".
			ss >> indice;
			indices.push_back(indice);

			return indices;
		};

		std::string line;
		while (objFile.good() && std::getline(objFile, line))
		{
			std::string sectionName;
			std::stringstream ss(line);


			ss >> sectionName;
			std::string dataLeft = ss.str().substr(sectionName.size() + 1, ss.str().size() - sectionName.size());


			if (sectionName == verticePositionsOption)
			{
				const auto& floats = extractFloats(dataLeft);
				points.push_back({ floats[0], floats[1], floats[2] });
			}
			else if (sectionName == textureCoordinatesOption)
			{
				const auto& floats = extractFloats(dataLeft);
				textureCoordinates.push_back({ floats[0], floats[1] });
			}
			else if (sectionName == normalsOption)
			{
				const auto& floats = extractFloats(dataLeft);
				normals.push_back({ floats[0], floats[1], floats[2] });
			}
			else if (sectionName == verticeDataOption)
			{
				std::string indicesString;

				for (int i = 0; i < 3; i++)
				{
					ss >> indicesString;

					auto verticeIndices = getIndicesFromIndicesString(indicesString);

					Vertice3D vertice;
					vertice.coordinates = points[verticeIndices[0] - 1];
					vertice.textureCoordinates = textureCoordinates[verticeIndices[1] - 1];

					shape->normals.push_back(normals[verticeIndices[2] - 1]);
					shape->vertices.push_back(vertice);
				}
			}
			else
			{
				logging::info("Parsing OBJ file. Skipping line", ss.str());
			}
		}
		objFile.close();
		return shape;
	}
}