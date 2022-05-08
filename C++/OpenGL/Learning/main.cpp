#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <numbers>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "src/headers/mygl/debug/debug.h"
#include "src/headers/mygl/texture.h"
#include "src/headers/mygl/shader/shader.h"
#include "src/headers/drawable/shape3d.h"
#include "src/headers/drawable/primitive.h"
#include "src/headers/transformation.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

/*
namespace control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
*/

class TheProgram
{
	typedef uint64_t ShapeID;
	typedef drawable::shape3d::Shape Shape3D;
	typedef drawable::primitive::Point3D Point3D;

	GLFWwindow* window = nullptr;

	std::unordered_map<ShapeID, std::unique_ptr<Shape3D>> shapes;

	MyGLShader shader;

	// TODO [PERFORMANCE]: Swap to index buffers.
	unsigned int VBO = 0, VAO = 0;

	// Map key number to callback function.
	// std::unordered_map<int, std::function<void()>> keyboardCallbacks;

	// Transformations.
	// std::unordered_map<ShapeID, vector<transformation3d::Transformer*>> complexShapesTransformers;
	///


	/// @brief See Shape::ComplexShape.convertToVertices()
	// Vertices convertVisibleComplexShapesToCoordinates()
	/*{
		// Collect visible complex shapes.
		const size_t amountOfComplexShapes = this->complexShapes.size() + this->complexShapesMap.size();
		vector<const shape::complex::ComplexShape*> visibleComplexShapes;
		visibleComplexShapes.reserve(amountOfComplexShapes);

		for (const auto& complexShape : this->complexShapes)
			if (complexShape->isVisible())
				visibleComplexShapes.emplace_back(complexShape);

		for (const auto& [complexShapeName, complexShape] : this->complexShapesMap)
			if (complexShape->isVisible())
				visibleComplexShapes.emplace_back(complexShape);

		// Count total amount of visible triangles.
		size_t amountOfTriangles = 0;

		for (const auto& complexShape : visibleComplexShapes)
			amountOfTriangles += complexShape->amountOfTriangles;

		// Convert amount of triangles to amount of vertices.
		const size_t amountOfVertices = amountOfTriangles * kCoordinatesPerTriangle;

		// Agregate vertices into allCoordinates.
		Vertices allCoordinates = { new float[amountOfVertices] , amountOfVertices };
		size_t destinationOffset = 0;

		/// @brief Copy sourceComplexShape vertices to destination + destinationOffset.
		///
		/// @return amount of copied vertices
		for (const shape::complex::ComplexShape* complexShape : visibleComplexShapes)
		{
			const Vertices& complexShapeCoordinates = complexShape->convertToVertices();

			const float* complexShapeCoordinatesStart = complexShapeCoordinates.vertices;
			const float* complexShapeCoordinatesEnd = complexShapeCoordinatesStart + complexShapeCoordinates.amountOfVertices;

			std::copy(complexShapeCoordinatesStart, complexShapeCoordinatesEnd, allCoordinates.vertices + destinationOffset);

			destinationOffset += complexShapeCoordinates.amountOfVertices;
		}

		ASSERT(amountOfVertices == destinationOffset);

		return allCoordinates;
	}
	*/

	// glm::f32* transformersMatrixes;

	// Old drawing
	//void draw()
	/* {
		struct ShapeToDraw
		{
			ShapeID shapeID;
			std::shared_ptr<complex2d::shape::ComplexShape> complexShape;
			std::shared_ptr<primitive2d::Vertices> vertices;
		};

		this->clearScreen();

		vector<ShapeToDraw> shapesToDraw;

		// TODO [PERFORMANCE]: Cache amount of visible shapes.
		shapesToDraw.reserve(this->complexShapes.size());

		size_t totalAmountOfVertices = 0;
		for (const auto& [complexShapeID, complexShape] : this->complexShapes)
		{
			if (complexShape->isVisible())
			{
				std::shared_ptr<primitive2d::Vertices> shapeVertices = complexShape->getVertices();

				totalAmountOfVertices += shapeVertices->amount;

				shapesToDraw.emplace_back(ShapeToDraw{ complexShapeID, complexShape, shapeVertices });
			}
		}

		// TODO [PERFORMANCE]: Cache vertices.
		primitive2d::Vertex* allVertices = new primitive2d::Vertex[totalAmountOfVertices];
		{
			size_t offset = 0;
			for (const auto& [complexShapeID, complexShape, vertices] : shapesToDraw)
			{
				std::copy(vertices->vertices, vertices->vertices + vertices->amount, allVertices + offset);
				offset += vertices->amount;
			}
		}

		// TODO [PERFORMANCE]: Move to update.
		myGLCall(glBufferData(GL_ARRAY_BUFFER,
			sizeof(primitive2d::Vertex) * totalAmountOfVertices,
			static_cast<const void*>(allVertices),
			GL_STATIC_DRAW
		));


		/* Actual drawing. *_/
		GLint offset = 0;
		for (const auto& [complexShapeID, complexShape, vertices] : shapesToDraw)
		{
			const auto& colorRGB = complexShape->getColorRGB();
			const float& alfa = 0.f;
			this->shader.setGLUniform4f("u_colorRGBA", colorRGB.red, colorRGB.green, colorRGB.blue, alfa);

			if (this->complexShapesTransformers.contains(complexShapeID))
			{
				const auto& transformers = this->complexShapesTransformers.at(complexShapeID);

				size_t offset = 0;
				for (size_t idx = 0; idx < transformers.size(); idx++)
				{
					const auto& transformationMatrix = transformers[idx]->calculateTransformationMatrix();
					std::copy(transformationMatrix, transformationMatrix + sizeof(glm::f32) * 16, transformersMatrixes + offset);
					offset += 16;
				}

				ASSERT(transformers.size() <= 4);

				this->shader.setGLlUniformMatrix4fv("u_transformations", transformersMatrixes, (GLsizei)transformers.size());
				this->shader.setGLlUniform1i("u_transformationsAmount", (const GLint)transformers.size());
			}
			else
				this->shader.setGLlUniform1i("u_transformationsAmount", (const GLint)0);

			myGLCall(glDrawArrays(GL_TRIANGLES,
				(GLint)offset,  // Offset.
				static_cast<GLsizei>(vertices->amount)
			));

			offset += vertices->amount;
		}
		delete[] allVertices;

		myGLCall(glfwSwapBuffers(window));
	}*/

	// Copy shapes data to the GPU buffer. TODO [OPTIMIZATION]: Do it on update().
	// Draw all shapes: one shape at the time.
	void draw()
	{
		struct ShapeToDraw
		{
			const ShapeID shapeID;
			const Shape3D* const& shape;
			const vector<Point3D> points;
		};

		// TODO [PERFORMANCE]: Cache shapes and update in update().
		vector<ShapeToDraw> shapesToDraw;
		shapesToDraw.reserve(this->shapes.size());

		// Fill shapesToDraw and count total amount of vertices.
		size_t totalAmountOfVertices = 0;
		for (const auto& [shapeID, shape] : this->shapes)
		{
			const vector<Point3D>& shapeVertices = shape->getVertices();

			totalAmountOfVertices += shapeVertices.size();

			shapesToDraw.push_back(ShapeToDraw{ shapeID, shape.get(), shapeVertices });
		}

		// TODO [PERFORMANCE]: Cache vertices.
		Point3D* allPoints = new Point3D[totalAmountOfVertices];
		{
			size_t offset = 0;
			for (const auto& [shapeID, shape, points] : shapesToDraw)
			{
				std::copy(points.begin(), points.end(), allPoints + offset);
				offset += points.size();
			}
		}

		// TODO [PERFORMANCE]: Move to update.
		myGLCall(glBufferData(GL_ARRAY_BUFFER,
			sizeof(Point3D) * totalAmountOfVertices,
			static_cast<const void*>(allPoints),
			GL_STATIC_DRAW
		));

		this->clearScreen();

		// Actual drawing. 
		GLint offset = 0;
		for (const auto& [shapeID, shape, points] : shapesToDraw)
		{
			const color::RGBA& color = shape->getColor();
			this->shader.setGLUniform4f("u_colorRGBA", color.red, color.green, color.blue, color.alfa);

			// Transformations.
			/*
			if (this->complexShapesTransformers.contains(complexShapeID))
			{
				const auto& transformers = this->complexShapesTransformers.at(complexShapeID);

				size_t offset = 0;
				for (size_t idx = 0; idx < transformers.size(); idx++)
				{
					const auto& transformationMatrix = transformers[idx]->calculateTransformationMatrix();
					std::copy(transformationMatrix, transformationMatrix + sizeof(glm::f32) * 16, transformersMatrixes + offset);
					offset += 16;
				}

				ASSERT(transformers.size() <= 4);

				this->shader.setGLlUniformMatrix4fv("u_transformations", transformersMatrixes, (GLsizei)transformers.size());
				this->shader.setGLlUniform1i("u_transformationsAmount", (const GLint)transformers.size());
			}
			else
				this->shader.setGLlUniform1i("u_transformationsAmount", (const GLint)0);
			*/

			myGLCall(glDrawArrays(GL_TRIANGLES,
				(GLint)offset,  // Offset.
				static_cast<GLsizei>(points.size())
			));

			offset += points.size();
		}

		myGLCall(glfwSwapBuffers(window));

		delete[] allPoints;
	}

	void clearScreen()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	//void setCallbacks()
	/*
	{
		glfwSetWindowUserPointer(this->window, reinterpret_cast<void*>(this));

		glfwSetScrollCallback(this->window, control::scrollCallback);
		glfwSetKeyCallback(this->window, control::keyboardCallback);
	}*/

	void handleEvents()
	{
		glfwPollEvents();
	}

public:
	void init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(config::kScreenWidth, config::kScreenHeight, config::kWindowName.c_str(), nullptr, nullptr);

		ASSERT(window != nullptr);  // Failed to create GLFW window.

		glfwMakeContextCurrent(window);

		ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));  // Failed to initialize GLAD.

		myGLCall(this->shader.createProgram());
		myGLCall(glUseProgram(this->shader.getShaderProgramID()));

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glBindVertexArray(VAO));

		myGLCall(glGenBuffers(1, &VBO));
		myGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

		// this->setCallbacks();

		myGLCall(glfwSwapInterval(2));

		// this->transformersMatrixes = new glm::f32[4 * 16];
	}
	//void setComplexShapeActive(int shapeID)

	void update()
	{
		//myGLCall(glBufferData(GL_ARRAY_BUFFER,
		//	sizeof(float) * this->coordinatesDataForGPU.amountOfVertices,
		//	this->coordinatesDataForGPU.vertices,
		//	GL_STATIC_DRAW
		//));

		constexpr GLint kCoordinates = 0;
		myGLCall(glEnableVertexAttribArray(kCoordinates));
		myGLCall(glVertexAttribPointer(kCoordinates, 3, GL_FLOAT, GL_FALSE, sizeof(Point3D), (void*)0));
	}

	/// @brief Add a circle to the scene.
	/// @param amountOfTriangles 
	/// @param radius 
	/// @return id of circle
	/*
	int addCircle(const size_t amountOfTriangles, const float radius, const primitive2d::Point& position = { 0,0 })
	{
		std::shared_ptr<complex2d::shape::Circle> newCircle = std::make_shared<complex2d::shape::Circle>(amountOfTriangles, radius, position);

		int circleID = static_cast<int>(complexShapes.size());

		complexShapes[circleID] = newCircle;

		return circleID;
	}

	int addTriangle(float sideLength, const primitive2d::Point& position, color::RGB color)
	{
		std::shared_ptr<complex2d::shape::Triangle> newTriangle = std::make_shared<complex2d::shape::Triangle>(sideLength, position, color);

		int triangleID = static_cast<int>(complexShapes.size());

		complexShapes[triangleID] = newTriangle;

		return triangleID;
	}

	void updateCircle(int circleID, size_t amountOfTriangles = 0, float radius = -1, color::RGB color = { -1 })
	{
		ASSERT(circleID < this->complexShapes.size());

		auto circle = static_cast<complex2d::shape::Circle*>(this->complexShapes[circleID].get());

		if (color.red >= 0)
			circle->updateCircle(color);


		if (amountOfTriangles > 0)
		{
			if (radius >= 0)
				circle->updateCircle(amountOfTriangles, radius);
			else
				circle->updateCircle(amountOfTriangles);
		}

		this->update();
	}

	void updateCircle(int circleID, color::RGB color = { -1 })
	{
		this->updateCircle(circleID, 0, -1, color);
	}

	*/

	ShapeID addHexahedron(float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA)
	{
		// TODO [OPTIMIZATION]: Swap to random id.
		ShapeID shapeID = 0;
		while (this->shapes.contains(shapeID))
			shapeID++;

		this->shapes[shapeID] = std::make_unique<drawable::shape3d::Hexahedron>(sideLength, position, color);

		return shapeID;
	}

	void mainLoop()
	{
		this->update();

		while (!glfwWindowShouldClose(window))
		{
			this->draw();
			this->handleEvents();
		}
	}

	// Transformations.
	/*
	void addTransformation(
		const ShapeID complexShapeID,
		const transformation3d::TransformatingType& tansformerType,
		const Vector transformationVector)
	{
		using namespace transformation3d;

		Transformer* transformer;
		switch (tansformerType)
		{
		case TransformatingType::Slide:
			transformer = new Slider(transformationVector);
			break;
		case TransformatingType::ContinousSlide:
			transformer = new continous::Slider(transformationVector);
			break;
		case TransformatingType::ContinousScale:
			transformer = new continous::Scaler(transformationVector);
			break;
		case TransformatingType::ContinousRotate:
			transformer = new continous::Rotator(transformationVector);
			break;
		default:
			throw;
		}

		this->complexShapesTransformers[complexShapeID].push_back(transformer);
	}
	*/
	///

	~TheProgram()
	{
		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteBuffers(1, &VBO));
		myGLCall(glDeleteProgram(this->shader.getShaderProgramID()));

		glfwTerminate();

		// Transformations.
		/*
		for (auto& [complexShape, vectorOfTransformations] : this->complexShapesTransformers)
			for (auto& transformation : vectorOfTransformations)
				delete transformation;
		*/
		///
	}
};

/*
namespace control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
		//const shape::complex::Circle* circle = static_cast<const shape::complex::Circle*>(program->getComplexShape(program->activeComplexShapes));

		//if (yoffset < 0 && circle->amountOfTriangles <= 3)
		//	return;

		//program->updateCircle(program->activeComplexShapes, circle->amountOfTriangles + int(yoffset));

		program->update();
	}

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
		const auto& keyboardCallbacks = program->getKeyboardCallbacks();

		if (action == GLFW_PRESS)
		{
			if (keyboardCallbacks.contains(key))
			{
				keyboardCallbacks.find(key)->second();
				program->update();
			}
			else
				cout << "Detected unhandled keyboard event: " << key << endl;
		}
	}
};
*/

int main()
{
	TheProgram program;
	program.init();

	program.addHexahedron(1.f);

	program.mainLoop();
	return 0;
}
