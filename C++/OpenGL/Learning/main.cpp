#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <random>
#include <iostream>
#include <numbers>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <list>
#include <functional>

#include "src/headers/mygl/debug/debug.h"
#include "src/headers/mygl/texture.h"
#include "src/headers/mygl/shader/shader.h"
#include "src/headers/drawable/drawable.h"
#include "src/headers/drawable/obj_loader.h"
#include "src/headers/drawable/primitive.h"
#include "src/headers/transformation.h"
#include "src/headers/model_view_projection.h"


namespace control
{
	void mouseMoveCallback(GLFWwindow* window, double posX, double posY);
	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};


class TheProgram
{
private:
	typedef drawable::primitive::Point2D Point2D;
	typedef drawable::shape2d::Circle Circle;
	
	bool isBallActive = false;
	Point2D ballMovingVector;

	GLFWwindow* window = nullptr;

	typedef uint64_t ShapeID;

	// Shapes.
	std::unordered_map<ShapeID, std::shared_ptr<Circle>> shapes;

	// Events.
	// TODO: merge it into one with enum fron event
	typedef int KeyID;
	std::unordered_map<KeyID, std::function<void()>> keyboardOnPressRepeatEvents;
	std::unordered_map<KeyID, std::function<void()>> keyboardOnPressEvents;
	std::vector<std::pair<std::string, std::function<void()>>> mouseOnMoveEvents;

	// Shaders.
	MyGLShader circleShader;

	// TODO [REFACTOR]: Move it to VertexArrayManager / VAO Manager.
	// TODO [PERFORMANCE]: Swap to index buffers.
	unsigned int VBO = -1, VAO = -1;

	// In seconds.
	float lastFrameRenderDeltaTime = 0.f;
	// Time from glfwGetTime() received after render of a frame. In seconds.
	float lastStartFrameRenderTime = 0.f;

	typedef drawable::primitive::Point2D Point2D;
	typedef drawable::primitive::Vertice2D Vertice2D;

	// TODO [REFACTOR]: Move to seperate somewhere.
	class PerformanceAnalyzer
	{
	public:
		float getAverageTimeToRender()
		{
			return renderTimeSum / float(kRenderTimeSampleSize);
		}

		float getAverageFPS()
		{
			return float(kRenderTimeSampleSize) / renderTimeSum;
		}

		void addRenderTime(float renderTime)
		{
			latestRenderTimeIdx++;

			if (latestRenderTimeIdx >= kRenderTimeSampleSize)
				latestRenderTimeIdx = 0;

			renderTimeSum -= renderTimes[latestRenderTimeIdx];
			renderTimeSum += renderTime;

			renderTimes[latestRenderTimeIdx] = renderTime;
		}
	private:
		const int kRenderTimeSampleSize = 1000;
		float renderTimes[1000]{ 1. };
		int latestRenderTimeIdx = 0;
		float renderTimeSum = 1.;  // Avoid dividing by zero.
	};

	PerformanceAnalyzer performanceAnalyzer;

	struct ShapesToDraw
	{
		size_t totalAmountOfVertices = 0;
		Vertice2D* allVertices = nullptr;

		struct ShapeToDraw
		{
			const ShapeID shapeID;
			const std::shared_ptr<Circle> shape;
			const std::vector<Vertice2D> vertices;
		};

		// TODO [PERFORMANCE]: Cache shapes and update in update().
		std::vector<ShapeToDraw> shapesToDraw;

		ShapesToDraw(const std::unordered_map<ShapeID, std::shared_ptr<Circle>>& shapes)
		{
			// Fill shapesToDraw and count total amount of vertices.
			this->shapesToDraw.reserve(shapes.size());
			for (const auto& [shapeID, shape] : shapes)
			{
				const auto& shapeVertices = shape->getVertices();

				totalAmountOfVertices += shapeVertices.size();

				this->shapesToDraw.push_back(ShapeToDraw{ shapeID, shape, shapeVertices });
			}

			// TODO [PERFORMANCE]: Cache vertices.

			// Allocate data and copy vertices data into one blob.
			this->allVertices = new Vertice2D[this->totalAmountOfVertices];

			size_t offset = 0;
			for (const auto& [shapeID, shape, vertices] : this->shapesToDraw)
			{
				std::copy(vertices.begin(), vertices.end(), allVertices + offset);
				offset += vertices.size();
			}

		}
		~ShapesToDraw()
		{
			delete this->allVertices;
		}
	};

	// Perfaps maybe dont ya think about refactoring THIS REEEEEAAALY BADLY? TODO [REFACTOR]: Seperate it to a class.
	// 1. Copy shapes data to the GPU buffer. TODO [OPTIMIZATION]: Do it on update().
	// 2. Draw all shapes: one shape at the time.
	void draw()
	{
		/// Prepare data to draw.

		// TODO [REFACTOR]: Find better place for it.
		// Draw polygons.
		const auto& drawShapes = [this](
			const std::unordered_map<ShapeID, std::shared_ptr<Circle>>& shapes) {

				if (this->isBallActive)
				{
					auto ball = shapes.at(0);

					const auto& ballCenter = ball->getCenter();
					const auto& ballRadius = ball->getRadius();
					
					const auto& setBallRandomColor = [this, &ball]()
					{
						ball->setColor({ this->generateRandomFloat01(), this->generateRandomFloat01(), this->generateRandomFloat01(), 1. });
					};

					if (ballCenter.x - ballRadius < -1)
					{
						ball->setCenter({ -1 + ballRadius, ballCenter.y });
						this->ballMovingVector.x *= -1;
						setBallRandomColor();
					}
					else if (ballCenter.x + ballRadius > 1)
					{
						ball->setCenter({ 1 - ballRadius, ballCenter.y });
						this->ballMovingVector.x *= -1;
						setBallRandomColor();
					}

					if (ballCenter.y - ballRadius < -1)
					{
						ball->setCenter({ ballCenter.x, -1 + ballRadius });
						this->ballMovingVector.y *= -1;
						setBallRandomColor();
					}
					else if (ballCenter.y + ballRadius > 1)
					{
						ball->setCenter({ ballCenter.x, 1 - ballRadius });
						this->ballMovingVector.y *= -1;
						setBallRandomColor();
					}
						
					logging::info("BALL_X", ballCenter.x);
					logging::info("BALL_Y", ballCenter.y);

					float multiplayer = lastFrameRenderDeltaTime;

					ball->move(this->ballMovingVector.x * multiplayer, this->ballMovingVector.y * multiplayer);
				}

				/// Prepare polygons.
				ShapesToDraw shapesToDraw(shapes);

				/// Prepare data.
				// _TODO [PERFORMANCE]: Move to update.
				myGLCall(glBindVertexArray(this->VAO));
				myGLCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO));
				this->setVertexAttribPointers();
				myGLCall(glUseProgram(this->circleShader.getShaderProgramID()));

				myGLCall(glBufferData(GL_ARRAY_BUFFER,
					sizeof(Vertice2D) * shapesToDraw.totalAmountOfVertices,
					static_cast<const void*>(shapesToDraw.allVertices),
					GL_STATIC_DRAW
				));

				// Actual drawing. 
				GLint verticesOffset = 0;
				for (const auto& [shapeID, shape, vertices] : shapesToDraw.shapesToDraw)
				{
					this->circleShader.setGLUniform4f("u_color", shape->getColor());

					// Draw triangle by triangle.
					constexpr int kAmountOfVerticesInTriangles = 3;
					for (auto i = 0; i < vertices.size(); i += kAmountOfVerticesInTriangles)
					{
						myGLCall(glDrawArrays(GL_TRIANGLES,
							(GLint)verticesOffset,
							static_cast<GLsizei>(kAmountOfVerticesInTriangles)
						));
						verticesOffset += kAmountOfVerticesInTriangles;
					}
				}
		};

		/// Start drawing.
		this->clearScreen();

		drawShapes(this->shapes);

		myGLCall(glfwSwapBuffers(window));

		// Show on title fps count and time to render.
		std::stringstream title;
		title << "FPS: " << performanceAnalyzer.getAverageFPS() << " "
			<< performanceAnalyzer.getAverageTimeToRender() << "ms";
		glfwSetWindowTitle(window, title.str().c_str());
	}

	void clearScreen()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glClear(GL_COLOR_BUFFER_BIT);
	}

	void setEvents()
	{
	}

	void setCallbacks()
	{
		glfwSetWindowUserPointer(this->window, reinterpret_cast<void*>(this));
		// glfwSetCursorPosCallback(this->window, control::mouseMoveCallback);
		glfwSetKeyCallback(this->window, control::keyboardCallback);
	}

	void handleEvents()
	{
		glfwPollEvents();

		// Hande keyboard.
		for (const auto& [key, callback] : this->keyboardOnPressRepeatEvents)
		{
			const auto& eventType = glfwGetKey(window, key);
			if (eventType == GLFW_PRESS || eventType == GLFW_REPEAT)
				callback();
		}

		// Handle mouse.
		for (const auto& callbackWithName : this->mouseOnMoveEvents)
			callbackWithName.second();
	}

	// TODO [REFACTOR]: Move vertex stuff somewhere.
	void setVertexAttribPointers()
	{
		// Coordinates.
		//const GLint kCoordsIdx = idx++;
		const GLint kCoordsIdx = 0;
		constexpr GLint kAmountOfCoords = 2;
		myGLCall(glEnableVertexAttribArray(kCoordsIdx));
		myGLCall(glVertexAttribPointer(
			kCoordsIdx,                                   // index
			kAmountOfCoords,	                          // size
			GL_FLOAT,			                          // type
			GL_FALSE,			                          // normalized
			sizeof(Vertice2D),		                      // stride
			(void*)(offsetof(Vertice2D, coordinates))));  // offset
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

		myGLCall(glEnable(GL_DEPTH_TEST));
		myGLCall(glFrontFace(GL_CCW));
		myGLCall(glEnable(GL_CULL_FACE));
		myGLCall(glCullFace(GL_BACK));

		this->circleShader.createProgramFromFile("src/mygl/shader/9.moving_circle.shader");

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glGenBuffers(1, &VBO));

		this->setCallbacks();
		this->setEvents();

		myGLCall(glfwSwapInterval(0));

		glfwSetTime(0.0);
	}

	float generateRandomFloat01() {
		static bool is_seeded = false;
		static std::mt19937 generator;

		// Seed once
		if (!is_seeded) {
			std::random_device rd;
			generator.seed(rd());
			is_seeded = true;
		}

		constexpr float max_range = 255;
		// Use mersenne twister engine to pick a random number
		// within the given range
		std::uniform_int_distribution<int> distribution(0, int(max_range));
		return distribution(generator) / max_range;
	}

	ShapeID addCircle(float radius, int amountOfTriangles = 100, Point2D position = { 0.f, 0.f }, color::RGBA color = color::kRedRGBA)
	{
		// TODO [PERFORAMNCE]: Swap to random id.
		// Find first avaiable id.
		ShapeID shapeID = 0;
		while (this->shapes.contains(shapeID))
			shapeID++;

		this->shapes[shapeID] = std::make_unique<Circle>(radius, amountOfTriangles, position, color);

		return shapeID;
	}

	void activateBall()
	{
		// if (this->isBallActive)
		// 	return;
		this->isBallActive = true;
		this->ballMovingVector = { generateRandomFloat01(), generateRandomFloat01()};
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			// Update frame time.
			{
				const float& now = float(glfwGetTime());
				this->lastFrameRenderDeltaTime = now - this->lastStartFrameRenderTime;
				this->lastStartFrameRenderTime = now;
			}
			performanceAnalyzer.addRenderTime(this->lastFrameRenderDeltaTime);

			this->handleEvents();
			this->draw();
		}
	}

	~TheProgram()
	{
		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteBuffers(1, &VBO));
		myGLCall(glDeleteProgram(this->circleShader.getShaderProgramID()));

		glfwTerminate();
	}
};


namespace control
{
	void mouseMoveCallback(GLFWwindow* window, double posX, double posY)
	{
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
		// program->update();
	}

	void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Use glfwSetKeyCallback?
		TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));

		if (action == GLFW_RELEASE)
		{
			if (key == GLFW_KEY_SPACE)
			{
				logging::info("BALL", "Releasing");
				program->activateBall();
			}
		}
	}
};

int main()
{
	TheProgram program;
	program.init();

	// The Cube.
	const float radius = 0.1f;
	const auto& circleID = program.addCircle(radius, 100);
	
	program.mainLoop();
	return 0;
}
