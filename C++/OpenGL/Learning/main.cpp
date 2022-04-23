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
#include "src/headers/shape/complex.h"
#include "src/headers/transformation.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

class TheProgram
{
	typedef uint16_t ShapeID;

	GLFWwindow* window = nullptr;

	vector<shape::complex::ComplexShape*> complexShapes;
	std::unordered_map<string, shape::complex::ComplexShape*> complexShapesMap;
	Coordinates coordinatesDataForGPU;

	MyGLShader shader;

	//?
	unsigned int VBO = 0, VAO = 0;

	// Map key number to callback function.
	std::unordered_map<int, std::function<void()>> keyboardCallbacks;

	std::unordered_set<int> activeComplexShapes;
	///

	// Transformations.
	std::unordered_multimap<ShapeID, transformation3d::Transformer*> complexShapesTransformers;
	///

	/// @brief See Shape::ComplexShape.convertToCoordinates()
	Coordinates convertVisibleComplexShapesToCoordinates()
	{
		// Collect visible complex shapes.
		const size_t amountOfComplexShapes = this->complexShapes.size() + this->complexShapesMap.size();
		vector<const shape::complex::ComplexShape*> visibleComplexShapes;
		visibleComplexShapes.reserve(amountOfComplexShapes);

		for (const auto& complexShape : this->complexShapes)
			if (complexShape->getVisibility())
				visibleComplexShapes.emplace_back(complexShape);

		for (const auto& [complexShapeName, complexShape] : this->complexShapesMap)
			if (complexShape->getVisibility())
				visibleComplexShapes.emplace_back(complexShape);

		// Count total amount of visible triangles.
		size_t amountOfTriangles = 0;

		for (const auto& complexShape : visibleComplexShapes)
			amountOfTriangles += complexShape->amountOfTriangles;

		// Convert amount of triangles to amount of coordinates.
		const size_t amountOfCoordinates = amountOfTriangles * kCoordinatesPerTriangle;

		// Agregate coordinates into allCoordinates.
		Coordinates allCoordinates = { new float[amountOfCoordinates] , amountOfCoordinates };
		size_t destinationOffset = 0;

		/// @brief Copy sourceComplexShape coordinates to destination + destinationOffset.
		/// 
		/// @return amount of copied coordinates
		for (const shape::complex::ComplexShape* complexShape : visibleComplexShapes)
		{
			const Coordinates& complexShapeCoordinates = complexShape->convertToCoordinates();

			const float* complexShapeCoordinatesStart = complexShapeCoordinates.coordinates;
			const float* complexShapeCoordinatesEnd = complexShapeCoordinatesStart + complexShapeCoordinates.amountOfCoordinates;

			std::copy(complexShapeCoordinatesStart, complexShapeCoordinatesEnd, allCoordinates.coordinates + destinationOffset);

			destinationOffset += complexShapeCoordinates.amountOfCoordinates;
		}

		ASSERT(amountOfCoordinates == destinationOffset);

		return allCoordinates;
	}

	void draw()
	{
		for (auto& [complexShape, transformer] : this->complexShapesTransformers)
		{
			this->clearScreen();

			this->shader.setGLlUniformMatrix4fv("u_model", transformer->calculateTransformationMatrix());

			glDrawArrays(GL_TRIANGLES, 
				0, 
				static_cast<GLsizei>(coordinatesDataForGPU.amountOfCoordinates)
			);

			glfwSwapBuffers(window);
		}
	}

	void clearScreen()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void setCallbacks()
	{
		glfwSetWindowUserPointer(this->window, reinterpret_cast<void*>(this));

		glfwSetScrollCallback(this->window, control::scrollCallback);
		glfwSetKeyCallback(this->window, control::keyboardCallback);
	}

	void handleEvents()
	{
		glfwPollEvents();
	}

public:
	std::unordered_set<int> getActiveComplexShapes()
	{
		return this->activeComplexShapes;
	}

	const shape::complex::ComplexShape* getComplexShape(int complexShapeID)
	{
		return this->complexShapes[complexShapeID];
	}

	void init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(config::kScreenWidth, config::kScreenHeight, config::kWindowName.c_str(), NULL, NULL);

		ASSERT(window != NULL);  // Failed to create GLFW window.

		glfwMakeContextCurrent(window);

		ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));  // Failed to initialize GLAD.

		this->shader.createProgram();
		myGLCall(glUseProgram(this->shader.getShaderProgramID()));

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glBindVertexArray(VAO));

		myGLCall(glGenBuffers(1, &VBO));
		myGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

		myGLCall(glEnableVertexAttribArray(0));


		myGLCall(auto u_colorID = glGetUniformLocation(this->shader.getShaderProgramID(), "u_colorRGB"));
		ASSERT(u_colorID != -1);  // Uniform not found.
		myGLCall(glUniform4f(u_colorID, 0.0f, 1.0f, 0.0f, 1.0f));

		auto u_model = glGetUniformLocation(this->shader.getShaderProgramID(), "u_model");
		myGLCall(ASSERT(u_colorID != -1));  // Uniform not found.

		// this->setCallbacks();

		myGLCall(glfwSwapInterval(2));
	}

	void setComplexShapeActive(int shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		this->activeComplexShapes.emplace(shapeID);
	}

	void update()
	{
		delete[] this->coordinatesDataForGPU.coordinates;

		this->coordinatesDataForGPU = this->convertVisibleComplexShapesToCoordinates();

		myGLCall(glBufferData(GL_ARRAY_BUFFER,
			sizeof(float) * this->coordinatesDataForGPU.amountOfCoordinates,
			this->coordinatesDataForGPU.coordinates,
			GL_STATIC_DRAW
		));

		myGLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	}

	/// @brief Add a circle to the scene.
	/// @param amountOfTriangles 
	/// @param radius 
	/// @return id of circle
	int addCircle(const size_t amountOfTriangles, const float radius,
		const shape::primitive::Point& position = { 0,0,0 })
	{
		shape::complex::Circle* newCircle = new shape::complex::Circle{ amountOfTriangles, radius, position };

		int circleID = static_cast<int>(complexShapes.size());

		complexShapes.emplace_back(newCircle);

		return circleID;
	}

	void deleteComplexShape(int shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		delete this->complexShapes[shapeID];
		this->complexShapes.erase(this->complexShapes.begin() + shapeID);
	}

	void updateCircle(int circleID, size_t amountOfTriangles = 0,
		float radius = -1, color::RGB color = { -1 })
	{
		ASSERT(circleID < this->complexShapes.size())

			shape::complex::Circle* circle = static_cast<shape::complex::Circle*>(this->complexShapes[circleID]);

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

	void mainLoop()
	{
		this->update();

		while (!glfwWindowShouldClose(window))
		{
			this->draw();
			this->handleEvents();
		}
	}

	void toggleShapeVisibility(int complexShapeID)
	{
		ASSERT(complexShapeID < this->complexShapes.size());

		shape::complex::ComplexShape*& shape = this->complexShapes[complexShapeID];
		shape->setVisibility(!shape->getVisibility());
	}

	void toggleShapeVisibilityOnKey(int complexShapeID, int key)
	{
		std::function<void()> func = [this, complexShapeID] {
			this->toggleShapeVisibility(complexShapeID);
		};

		this->keyboardCallbacks[key] = func;
	}

	auto getKeyboardCallbacks() const
	{
		return this->keyboardCallbacks;
	}

	// Transformations.
	void addTransformation(const ShapeID complexShapeID,
		const transformation3d::TransformatingType& tansformerType,
		const Vector transformationVector)
	{
		using namespace transformation3d;
		Transformer* transformer;
		switch (tansformerType)
		{
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

		this->complexShapesTransformers.insert({ complexShapeID, transformer });
	}
	///

	~TheProgram()
	{
		for (const auto& complexShape : this->complexShapes)
			delete complexShape;

		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteBuffers(1, &VBO));

		glfwTerminate();

		// Transformations.
		for (auto& [complexShape, transformation] : this->complexShapesTransformers)
			delete transformation;
		///
	}
};

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

int main()
{
	TheProgram program;
	program.init();

	auto triangle1ID = program.addCircle(3, 0.2f, { -0.7f, 0.7f });
	auto triangle2ID = program.addCircle(3, 0.2f, { -0.7f, -0.7f });
	auto triangle3ID = program.addCircle(3, 0.2f, { 0.7f, -0.7f });
	auto triangle4ID = program.addCircle(3, 0.2f, { 0.7f, 0.7f });

	program.updateCircle(triangle1ID, { 1.f, 0.f, 0.f });
	program.updateCircle(triangle2ID, { 0.f, 1.f, 0.f });
	program.updateCircle(triangle3ID, { 0.f, 0.f, 1.f });
	program.updateCircle(triangle4ID, { 1.f, 1.f, 0.f });

	program.setComplexShapeActive(triangle1ID);
	program.setComplexShapeActive(triangle2ID);
	program.setComplexShapeActive(triangle3ID);
	program.setComplexShapeActive(triangle4ID);

	program.addTransformation(triangle1ID, transformation3d::TransformatingType::ContinousSlide, { 1.f, 1.f, 1.f });
	program.addTransformation(triangle1ID, transformation3d::TransformatingType::ContinousSlide, { 0.2f, 0.f, 0.f });
	program.addTransformation(triangle1ID, transformation3d::TransformatingType::ContinousSlide, { 0.2f, 0.f, 0.f });
	program.addTransformation(triangle1ID, transformation3d::TransformatingType::ContinousSlide, { 0.2f, 0.f, 0.f });

	program.mainLoop();

	return 0;
}
