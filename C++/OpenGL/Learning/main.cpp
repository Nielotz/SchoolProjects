#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <numbers>
#include <vector>
#include <string>
#include <unordered_map>

#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#pragma warning(pop)

#include "src/headers/shape/complex.h"

#define ASSERT(x) if(!(x)) __debugbreak();

using std::cout;
using std::endl;
using std::string;
using std::vector;

void printErrorAndQuit(const char* errorMessage)
{
	std::cout << errorMessage << std::endl;
	exit(-1);
}

void printErrorAndQuit(const string& errorMessage)
{
	printErrorAndQuit(errorMessage.c_str());
}

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

namespace Control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

class TheProgram
{
	const GLchar* kVertexShaderSourceCode = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	const char* kFragmentShaderSourceCode = "#version 330 core\n"
		"layout(location = 0) out vec4 color;\n"
		"uniform vec4 u_ColorRGB;\n"
		"void main()\n"
		"{\n"
		"   color = u_ColorRGB;\n"
		"}\n\0";

	unsigned int VBO = 0, VAO = 0;

	GLFWwindow* window = nullptr;

	vector<shape::complex::ComplexShape*> complexShapes;
	std::unordered_map<string, shape::complex::ComplexShape*> complexShapesMap;

	Coordinates currentCoordinates;

	GLuint shaderProgram = 0;

	/// @brief See Shape::ComplexShape.convertToCoordinates()
	Coordinates convertComplexShapesToCoordinates()
	{
		// Count total amount of triangles.
		GLsizei amountOfTriangles = 0;

		for (const auto& complexShape : this->complexShapes)
			amountOfTriangles += complexShape->amountOfTriangles;

		for (const auto& [key, value] : this->complexShapesMap)
			amountOfTriangles += value->amountOfTriangles;

		// Convert amount of triangles to amount of coordinates.
		const GLsizei amountOfCoordinates = amountOfTriangles * kCoordinatesPerTriangle;

		// Agregate coordinates into allCoordinates.
		Coordinates allCoordinates = { new GLfloat[amountOfCoordinates] , amountOfCoordinates };
		GLsizei destinationOffset = 0;

		/// @brief Copy sourceComplexShape coordinates to destination + destinationOffset.
		/// 
		/// @return amount of copied coordinates
		auto copyComplexShapeCoordinates = [](const shape::complex::ComplexShape* const& sourceComplexShape, GLfloat* destination, GLsizei destinationOffset)
		{
			const Coordinates& complexShapeCoordinates = sourceComplexShape->convertToCoordinates();

			const GLfloat* complexShapeCoordinatesStart = complexShapeCoordinates.coordinates;
			const GLfloat* complexShapeCoordinatesEnd = complexShapeCoordinatesStart + complexShapeCoordinates.amountOfCoordinates;

			std::copy(complexShapeCoordinatesStart, complexShapeCoordinatesEnd, destination + destinationOffset);

			return complexShapeCoordinates.amountOfCoordinates;
		};

		for (const shape::complex::ComplexShape* complexShape : this->complexShapes)
			destinationOffset += copyComplexShapeCoordinates(complexShape, allCoordinates.coordinates, destinationOffset);

		for (const auto& [complexShapeName, complexShape] : this->complexShapesMap)
			destinationOffset += copyComplexShapeCoordinates(complexShape, allCoordinates.coordinates, destinationOffset);

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

		glDrawArrays(GL_TRIANGLES, 0, currentCoordinates.amountOfCoordinates);

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

		glfwSetScrollCallback(this->window, Control::scrollCallback);
		glfwSetKeyCallback(this->window, Control::keyboardCallback);
	}

	void handleEvents()
	{
		glfwPollEvents();
	}

public:
	bool isAnyComplexShapeActive = false;
	GLint activeComplexShapeID = 0;
	GLint u_activeComplexShapeID = 0;

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

		window = glfwCreateWindow(kScreenWidth, kScreenHeight, kWindowName, NULL, NULL);

		if (window == NULL)
			printErrorAndQuit("Failed to create GLFW window");

		glfwMakeContextCurrent(window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			printErrorAndQuit("Failed to initialize GLAD");

		this->compileShaders();
		glUseProgram(shaderProgram);

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glEnableVertexAttribArray(0);

		this->u_activeComplexShapeID = glGetUniformLocation(this->shaderProgram, "u_ColorRGB");

		if (this->u_activeComplexShapeID == -1)
			printErrorAndQuit("Uniform not found.");

		this->setCallbacks();

		glfwSwapInterval(1);
	}

	void setComplexShapeActive(GLint shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		this->activeComplexShapeID = shapeID;
		this->isAnyComplexShapeActive = true;
	}

	void update()
	{
		delete[] this->currentCoordinates.coordinates;

		this->currentCoordinates = this->convertComplexShapesToCoordinates();

		glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * this->currentCoordinates.amountOfCoordinates,
			this->currentCoordinates.coordinates,
			GL_STATIC_DRAW
		);

		const shape::complex::Circle* circle = static_cast<const shape::complex::Circle*>(this->complexShapes[this->activeComplexShapeID]);
		const color::RGB& color = circle->getColorRGB();

		glUniform4f(this->u_activeComplexShapeID, color.red, color.green, color.blue, 1.0f);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	}

	/// @brief Add a circle to the scene.
	/// @param amountOfTriangles 
	/// @param radius 
	/// @return id of circle
	GLint addCircle(const GLsizei amountOfTriangles, const GLfloat radius)
	{
		shape::complex::Circle* newCircle = new shape::complex::Circle{ amountOfTriangles, radius };

		GLint circleID = static_cast<GLint>(complexShapes.size());

		complexShapes.emplace_back(newCircle);

		//newCircle->printCoordinates();

		return circleID;
	}

	void deleteComplexShape(GLint shapeID)
	{
		ASSERT(shapeID < this->complexShapes.size());

		delete this->complexShapes[shapeID];
		this->complexShapes.erase(this->complexShapes.begin() + shapeID);
	}

	void updateCircle(GLint circleID, GLsizei amountOfTriangles = 0, GLfloat radius = -1, color::RGB color = { -1 })
	{
		if (this->complexShapes.size() <= circleID)
		{
			const string errorMessage = "Attempted to update circle with ID: " + std::to_string(circleID)
				+ " but only " + std::to_string(this->complexShapes.size()) + " exists.";
			printErrorAndQuit(errorMessage.c_str());
		}

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

		//circle->printCoordinates();

		this->update();
	}

	void updateCircle(GLint circleID, color::RGB color = { -1 })
	{
		this->updateCircle(circleID, 0, -1, color);
	}

	void removeShape()
	{

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

namespace Control
{
	void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
		const shape::complex::Circle* circle = static_cast<const shape::complex::Circle*>(program->getComplexShape(program->activeComplexShapeID));

		if (yoffset < 0 && circle->amountOfTriangles <= 3)
			return;

		program->updateCircle(program->activeComplexShapeID, circle->amountOfTriangles + int(yoffset));

		program->update();
	}

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
		// const Shape::Circle* circle = static_cast<const Shape::Circle*>(program->getComplexShape(program->activeComplexShapeID));

		// Show / hide triangle.
		if (key == GLFW_KEY_1)
		{
			//for (auto)
			//if (program)
			//program.
			int x = 2;
		}

		//this->updateCircle(this->activeComplexShapeID, ColorRGB{ 1.f, 0.f, 0.f });

		// else if (glfwGetKey(window, GLFW_KEY_2))
		//     this->updateCircle(this->activeComplexShapeID, ColorRGB{ 0.f, 1.f, 0.f });
		// else if (glfwGetKey(window, GLFW_KEY_3))
		//     this->updateCircle(this->activeComplexShapeID, ColorRGB{ 0.f, 0.f, 1.f });


		//program->updateCircle(program->activeComplexShapeID, circle->amountOfTriangles + yoffset);

		program->update();
	}
};

int main()
{
	TheProgram program;
	program.init();

	int amountOfTriangles = 20;
	const GLfloat kRadius = 0.5;

	GLint circleID = program.addCircle(amountOfTriangles, kRadius);
	program.setComplexShapeActive(circleID);

	program.mainLoop();

	return 0;
}
