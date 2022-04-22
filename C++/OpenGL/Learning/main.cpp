#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "src/headers/glm/glm.hpp"
#include "src/headers/glm/gtc/matrix_transform.hpp"
#include "src/headers/glm/gtc/type_ptr.hpp"

#include <iostream>
#include <numbers>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "src/headers/mygl/debug/debug.h"
#include "src/headers/shape/complex.h"
#include "src/headers/mygl/texture.h"
#include "src/headers/mygl/shader/shader.h"


using std::cout;
using std::endl;
using std::string;
using std::vector;


namespace control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

class Transformation
{
protected:
	glm::mat4 model = glm::mat4(1.0f);
	const float xDistanceToMove = 0.2f;

public:
	enum class TransformationType
	{
		MoveX,
		Rotate,
		Scale,
		All
	};

	const Transformation::TransformationType getTransportationType()
	{
		return TransformationType::MoveX;
	}

	void update()
	{}

	const glm::f32* getModel() const
	{
		return glm::value_ptr(this->model);
	}
private:
	Transformation::TransformationType transportationType;
};

class MoveX : public Transformation
{
public:
	Transformation::TransformationType transportationType = Transformation::TransformationType::MoveX;
	void update()
	{
		this->model = glm::translate(glm::mat4(1.0f), glm::vec3(xDistanceToMove * sin(glfwGetTime()), 0.0f, 0.0f));
	}
};

class Scale : public Transformation
{
public:
	Transformation::TransformationType transportationType = Transformation::TransformationType::Scale;
	void update()
	{
		this->model = glm::translate(glm::mat4(1.0f), glm::vec3(xDistanceToMove * sin(glfwGetTime()), 0.0f, 0.0f));
	}
};

class Rotate : public Transformation
{
public:
	Transformation::TransformationType transportationType = Transformation::TransformationType::Rotate;
	void update()
	{
		this->model = glm::rotate(glm::mat4(1.0f), 
			glm::radians(float(sin(glfwGetTime()) * 360)),
			glm::vec3(0.0f, 0.0f, 1.0f));
	}
};

class All : public Transformation
{
	Transformation::TransformationType transportationType = Transformation::TransformationType::All;
};


class TheProgram
{
	unsigned int VBO = 0, VAO = 0;

	GLFWwindow* window = nullptr;

	vector<shape::complex::ComplexShape*> complexShapes;
	std::unordered_map<string, shape::complex::ComplexShape*> complexShapesMap;

	Coordinates coordinatesDataForGPU;

	GLuint shaderProgram = 0;

	// Map key number to callback function.
	std::unordered_map<int, std::function<void()>> keyboardCallbacks;

	std::unordered_set<int> activeComplexShapes;
	std::unordered_map<int, Transformation*> transformationsComplexShapes;

	// Map ComplexShapeID to textureID.
	std::unordered_map<int, int> textures;

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
		for (auto & [complexShape, transformation] : this->transformationsComplexShapes)
		{
			this->clearScreen();

			if (transformation->getTransportationType() == Transformation::TransformationType::MoveX 
				|| transformation->getTransportationType() == Transformation::TransformationType::All)
				static_cast<MoveX*>(transformation)->update();
			if (transformation->getTransportationType() == Transformation::TransformationType::Rotate
				|| transformation->getTransportationType() == Transformation::TransformationType::All)
				static_cast<Rotate*>(transformation)->update();
			if (transformation->getTransportationType() == Transformation::TransformationType::Scale
				|| transformation->getTransportationType() == Transformation::TransformationType::All)
				static_cast<Scale*>(transformation)->update();

			glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, "u_model"), 1, GL_FALSE, transformation->getModel());

			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(coordinatesDataForGPU.amountOfCoordinates));

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

	void createShaderProgram()
	{
		this->shaderProgram = MyGLShader::createProgram();
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

		this->createShaderProgram();
		myGLCall(glUseProgram(shaderProgram));

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glBindVertexArray(VAO));

		myGLCall(glGenBuffers(1, &VBO));
		myGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

		myGLCall(glEnableVertexAttribArray(0));

		myGLCall(auto u_colorID = glGetUniformLocation(this->shaderProgram, "u_colorRGB"));
		ASSERT(u_colorID != -1);  // Uniform not found.
		myGLCall(glUniform4f(u_colorID, 0.0f, 1.0f, 0.0f, 1.0f));

		auto u_model = glGetUniformLocation(this->shaderProgram, "u_model");
		myGLCall(ASSERT(u_colorID != -1));  // Uniform not found.

		this->setCallbacks();

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

	void addTransformation(int complexShapeID, Transformation* transformation)
	{
		this->transformationsComplexShapes[complexShapeID] = transformation;
	}

	~TheProgram()
	{
		for (const auto& complexShape : this->complexShapes)
			delete complexShape;

		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteBuffers(1, &VBO));
		myGLCall(glDeleteProgram(shaderProgram));

		glfwTerminate();
	
		for (auto& [complexShape, transformation] : this->transformationsComplexShapes)
			delete transformation;
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

	int triangle1ID = program.addCircle(3, 0.2f, { -0.7f, 0.7f });
	int triangle2ID = program.addCircle(3, 0.2f, { -0.7f, -0.7f });
	int triangle3ID = program.addCircle(3, 0.2f, { 0.7f, -0.7f });
	int triangle4ID = program.addCircle(3, 0.2f, { 0.7f, 0.7f });

	program.updateCircle(triangle1ID, { 1.f, 0.f, 0.f });
	program.updateCircle(triangle2ID, { 0.f, 1.f, 0.f });
	program.updateCircle(triangle3ID, { 0.f, 0.f, 1.f });
	program.updateCircle(triangle4ID, { 1.f, 1.f, 0.f });

	program.setComplexShapeActive(triangle1ID);
	program.setComplexShapeActive(triangle2ID);
	program.setComplexShapeActive(triangle3ID);
	program.setComplexShapeActive(triangle4ID);

	program.addTransformation(triangle1ID, new MoveX);
	program.addTransformation(triangle2ID, new Rotate);
	program.addTransformation(triangle3ID, new Scale);
	program.addTransformation(triangle4ID, new All);

	program.mainLoop();

	return 0;
}
