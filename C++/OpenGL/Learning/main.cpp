#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <numbers>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "src/headers/shape/complex.h"
#include "src/headers/texture.h"

#define ASSERT(x) if(!(x)) __debugbreak();

using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace Shader
{
	GLuint compile(GLenum&& shaderType, const GLchar* sourceCode)
	{
		GLuint compiledShaderID = glCreateShader(shaderType);
		glShaderSource(compiledShaderID, 1, &sourceCode, NULL);
		glCompileShader(compiledShaderID);

		int success;
		glGetShaderiv(compiledShaderID, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(compiledShaderID, 512, NULL, infoLog);
			if (shaderType == GL_VERTEX_SHADER)
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			else if (shaderType == GL_FRAGMENT_SHADER)
				std::cout << "ERROR::SHADER::FRAGMENT::GL_FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			else
				std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw;
		}

		return compiledShaderID;
	}

	GLuint createProgram(vector<GLuint> shaders)
	{
		GLuint shaderProgram = glCreateProgram();

		for (auto& shader : shaders)
			glAttachShader(shaderProgram, shader);

		glLinkProgram(shaderProgram);

		int success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetProgramInfoLog(shaderProgram, 612, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		return shaderProgram;
	}
};

namespace control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

class TheProgram
{
	const GLchar* kVertexShaderSourceCode = "#version 330 core\n"
		"layout(location = 0) in vec3 position;\n"
		"layout(location = 1) in vec3 textureCoordinates;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
		"}\0";

	const char* kFragmentShaderSourceCode = "#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"uniform vec4 u_colorRGB;\n"
		"uniform sampler2D u_texture1;\n"
		"uniform sampler2D u_texture2;\n"
		"\n"
		"void main()\n"
		"{\n"
		"   vec4 = texture(u_texture1, texture1Coordinates);\n"
		"   color = u_colorRGB;\n"
		"}\n\0";

	unsigned int VBO = 0, VAO = 0;

	GLFWwindow* window = nullptr;

	vector<shape::complex::ComplexShape*> complexShapes;
	std::unordered_map<string, shape::complex::ComplexShape*> complexShapesMap;

	Coordinates coordinatesDataForGPU;

	GLuint shaderProgram = 0;

	// Map key number to callback function.
	std::unordered_map<int, std::function<void()>> keyboardCallbacks;

	std::unordered_set<GLint> activeComplexShapes;

	// Map ComplexShapeID to textureID.
	std::unordered_map<GLint, GLint> textures;

	GLint u_colorID = -1;
	GLint u_texture1ID = -1;
	GLint u_texture2ID = -1;

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
		GLsizei amountOfTriangles = 0;

		for (const auto& complexShape : visibleComplexShapes)
			amountOfTriangles += complexShape->amountOfTriangles;

		// Convert amount of triangles to amount of coordinates.
		const GLsizei amountOfCoordinates = amountOfTriangles * kCoordinatesPerTriangle;

		// Agregate coordinates into allCoordinates.
		Coordinates allCoordinates = { new GLfloat[amountOfCoordinates] , amountOfCoordinates };
		GLsizei destinationOffset = 0;

		/// @brief Copy sourceComplexShape coordinates to destination + destinationOffset.
		/// 
		/// @return amount of copied coordinates
		for (const shape::complex::ComplexShape* complexShape : visibleComplexShapes)
		{
			const Coordinates& complexShapeCoordinates = complexShape->convertToCoordinates();

			const GLfloat* complexShapeCoordinatesStart = complexShapeCoordinates.coordinates;
			const GLfloat* complexShapeCoordinatesEnd = complexShapeCoordinatesStart + complexShapeCoordinates.amountOfCoordinates;

			std::copy(complexShapeCoordinatesStart, complexShapeCoordinatesEnd, allCoordinates.coordinates + destinationOffset);

			destinationOffset += complexShapeCoordinates.amountOfCoordinates;
		}

		ASSERT(amountOfCoordinates == destinationOffset);

		return allCoordinates;
	}

	void compileShaders()
	{
		cout << "Compiling fragmentShader..." << endl;
		GLuint fragmentShader = Shader::compile(GL_FRAGMENT_SHADER, kFragmentShaderSourceCode);

		cout << "Compiling vertexShader..." << endl;
		GLuint vertexShader = Shader::compile(GL_VERTEX_SHADER, kVertexShaderSourceCode);

		cout << "Creating GPU program..." << endl;
		this->shaderProgram = Shader::createProgram({ fragmentShader , vertexShader });

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	void draw()
	{
		this->clearScreen();

		glDrawArrays(GL_TRIANGLES, 0, coordinatesDataForGPU.amountOfCoordinates);

		glfwSwapBuffers(window);
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
	std::unordered_set<GLint> getActiveComplexShapes()
	{
		return this->activeComplexShapes;
	}

	const shape::complex::ComplexShape* getComplexShape(GLint complexShapeID)
	{
		return this->complexShapes[complexShapeID];
	}

	void init()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(kScreenWidth, kScreenHeight, kWindowName.c_str(), NULL, NULL);

		ASSERT(window != NULL);  // Failed to create GLFW window.

		glfwMakeContextCurrent(window);

		ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));  // Failed to initialize GLAD.

		this->compileShaders();
		glUseProgram(shaderProgram);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(0);

		this->u_colorID = glGetUniformLocation(this->shaderProgram, "u_colorRGB");
		ASSERT(this->u_colorID != -1);  // Uniform not found.
		glUniform4f(this->u_colorID, 0.0f, 1.0f, 0.0f, 1.0f);

		this->u_texture1ID = glGetUniformLocation(this->shaderProgram, "u_texture1");
		ASSERT(this->u_texture1ID != -1);  // Uniform not found.
		glUniform1i(this->u_texture1ID, 0);

		this->u_texture2ID = glGetUniformLocation(this->shaderProgram, "u_texture2");
		ASSERT(this->u_texture2ID != -1);  // Uniform not found.
		glUniform1i(this->u_texture2ID, 1);

		this->setCallbacks();

		glfwSwapInterval(2);
	}

	void setComplexShapeActive(GLint shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		this->activeComplexShapes.emplace(shapeID);
	}

	void update()
	{
		delete[] this->coordinatesDataForGPU.coordinates;

		this->coordinatesDataForGPU = this->convertVisibleComplexShapesToCoordinates();

		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * this->coordinatesDataForGPU.amountOfCoordinates,
			this->coordinatesDataForGPU.coordinates,
			GL_STATIC_DRAW
		);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	}

	/// @brief Add a circle to the scene.
	/// @param amountOfTriangles 
	/// @param radius 
	/// @return id of circle
	GLint addCircle(const GLsizei amountOfTriangles, const GLfloat radius,
		const shape::primitive::Point& position = { 0,0,0 })
	{
		shape::complex::Circle* newCircle = new shape::complex::Circle{ amountOfTriangles, radius, position };

		GLint circleID = static_cast<GLint>(complexShapes.size());

		complexShapes.emplace_back(newCircle);

		return circleID;
	}

	void deleteComplexShape(GLint shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		delete this->complexShapes[shapeID];
		this->complexShapes.erase(this->complexShapes.begin() + shapeID);
	}

	void updateCircle(GLint circleID, GLsizei amountOfTriangles = 0,
		GLfloat radius = -1, color::RGB color = { -1 })
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

	void updateCircle(GLint circleID, color::RGB color = { -1 })
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

	void toggleShapeVisibility(GLint complexShapeID)
	{
		ASSERT(complexShapeID < this->complexShapes.size());

		shape::complex::ComplexShape*& shape = this->complexShapes[complexShapeID];
		shape->setVisibility(!shape->getVisibility());
	}

	void toggleShapeVisibilityOnKey(GLint complexShapeID, int key)
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

	~TheProgram()
	{
		for (const auto& complexShape : this->complexShapes)
			delete complexShape;

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(shaderProgram);

		glfwTerminate();
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

	GLint squareID = program.addCircle(4, 0.2f, { -0.7f, 0.7f });
	GLint triangleID = program.addCircle(3, 0.2f, { 0.7f, -0.7f });

	constexpr int kKey1 = 49;
	constexpr int kKey2 = 50;

	program.toggleShapeVisibilityOnKey(squareID, kKey1);
	program.toggleShapeVisibilityOnKey(triangleID, kKey2);

	program.setComplexShapeActive(squareID);
	program.setComplexShapeActive(triangleID);

	Texture texture1("res/textures/texture1.jpg");
	Texture texture2("res/textures/texture2.jpg");

	texture1.bind(0);
	texture2.bind(1);

	program.mainLoop();

	return 0;
}
