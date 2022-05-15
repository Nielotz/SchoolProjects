#include <glad/glad.h>
#include <GLFW/glfw3.h>

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
#include "src/headers/drawable/shape3d.h"
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
	GLFWwindow* window = nullptr;

	typedef uint64_t ShapeID;
	typedef drawable::shape3d::Shape Shape3D;

	// TODO [PERFORMANCE]: Group shapes stuff (transformations, textures, etc.) in one place.

	// Shapes.
	std::unordered_map<ShapeID, std::unique_ptr<Shape3D>> shapes;

	// Transformations.
	typedef std::vector<std::shared_ptr<transformation3d::Transformer>> VectorOfTransformers;
	std::unordered_map<ShapeID, VectorOfTransformers> shapesTransformers;

	// Textures.
	typedef MyGL::Texture2D Texture2D;
	std::unordered_map<ShapeID, std::shared_ptr<Texture2D>> shapesTextures;

	typedef int KeyID;
	std::unordered_map<KeyID, std::function<void()>> keyboardOnPressRepeatEvents;
	std::vector<std::pair<std::string, std::function<void()>>> mouseOnMoveEvents;

	MyGLShader shader;

	// TODO [PERFORMANCE]: Swap to index buffers.
	unsigned int VBO = 0, VAO = 0;

	MVP modelViewProjection;

	// In seconds.
	float lastFrameRenderDeltaTime = 0.f;
	// Time from glfwGetTime() received after render of a frame. In seconds.
	float lastStartFrameRenderTime = 0.f;

	typedef drawable::primitive::Point3D Point3D;
	typedef drawable::primitive::Vertice3D Vertice3D;

	// TODO [REFACTORING]: Move to seperate somewhere.
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
		float renderTimes[1000]{1.};
		int latestRenderTimeIdx = 0;
		float renderTimeSum = 1.;  // Avoid dividing by zero.
	};

	PerformanceAnalyzer performanceAnalyzer;

	// Perfaps maybe dont ya think about refactoring THIS?
	// 1. Copy shapes data to the GPU buffer. TODO [OPTIMIZATION]: Do it on update().
	// 2. Draw all shapes: one shape at the time.
	void draw()
	{
		/// Prepare data to draw.

		struct ShapeToDraw
		{
			const ShapeID shapeID;
			const Shape3D* const& shape;
			const std::vector<Vertice3D> vertices;
		};

		// TODO [PERFORMANCE]: Cache shapes and update in update().
		std::vector<ShapeToDraw> shapesToDraw;
		shapesToDraw.reserve(this->shapes.size());

		// Fill shapesToDraw and count total amount of vertices.
		size_t totalAmountOfVertices = 0;
		for (const auto& [shapeID, shape] : this->shapes)
		{

			// for (const auto& vertices : shape->getPoints())
			// {
			// 	std::cout << vertices.x << " " << vertices.y << " " << vertices.z << std::endl;
			// }

			// for (const auto& vertices : shape->getVertices())
			// {
			// 	//std::cout << vertices.coordinates.x << " " << vertices.coordinates.y << " " << vertices.coordinates.z << std::endl;
			// 	std::cout << vertices.textureCoordinates.x << " " << vertices.textureCoordinates.y << std::endl;
			// }

			const std::vector<Vertice3D>& shapeVertices = shape->getVertices();

			totalAmountOfVertices += shapeVertices.size();

			shapesToDraw.push_back(ShapeToDraw{ shapeID, shape.get(), shapeVertices });
		}

		// TODO [PERFORMANCE]: Cache vertices.
		Vertice3D* allVertices = new Vertice3D[totalAmountOfVertices];
		{
			size_t offset = 0;
			for (const auto& [shapeID, shape, vertices] : shapesToDraw)
			{
				std::copy(vertices.begin(), vertices.end(), allVertices + offset);
				offset += vertices.size();
			}
		}

		// TODO [PERFORMANCE]: Move to update.
		myGLCall(glBufferData(GL_ARRAY_BUFFER,
			sizeof(Vertice3D) * totalAmountOfVertices,
			static_cast<const void*>(allVertices),
			GL_STATIC_DRAW
		));

		this->clearScreen();

		this->shader.setGLlUniformMat4f("u_MVP", this->modelViewProjection.getMVP());

		// Actual drawing. 
		GLint verticesOffset = 0;
		for (const auto& [shapeID, shape, vertices] : shapesToDraw)
		{
			// Textures.
			// TODO [FUNCTIONAL]: Add default texture.
			ASSERT(this->shapesTextures.contains(shapeID));
			this->shader.setGLlUniform1i("u_texture", this->shapesTextures.at(shapeID)->getSlot());

			// Transformations.
			glm::mat4 transformationMatrix(1.f);
			if (this->shapesTransformers.contains(shapeID))
				for (const auto& transformer : this->shapesTransformers.at(shapeID))
					transformationMatrix *= transformer->calculateTransformationMatrix();

			this->shader.setGLlUniformMat4f("u_transformation",
				transformationMatrix);

			// Draw triangle by triangle.
			constexpr int kAmountOfVerticesInTriangles = 3;
			for (auto i = 0; i < vertices.size(); i += kAmountOfVerticesInTriangles)
			{
				// Set test colors.
				//const color::RGBA& color = shape->getColor();
				// this->shader.setGLUniform4f("u_colorRGBA", color.red, color.green + i / 50.f, color.blue + i / 50.f, color.alfa);

				myGLCall(glDrawArrays(GL_TRIANGLES,
					(GLint)verticesOffset,  // Offset.
					static_cast<GLsizei>(kAmountOfVerticesInTriangles)
				));
				verticesOffset += kAmountOfVerticesInTriangles;
			}
		}

		myGLCall(glfwSwapBuffers(window));

		// Show on title fps count and time to render.
		std::stringstream title;
		title << "FPS: " << performanceAnalyzer.getAverageFPS() << " "
			<< performanceAnalyzer.getAverageTimeToRender() << "ms";
		glfwSetWindowTitle(window, title.str().c_str());

		delete[] allVertices;
	}

	void clearScreen()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// glClear(GL_COLOR_BUFFER_BIT);
	}

	void setEvents()
	{
		// Keyboard.
		const auto& getMoveDistance = [this]() -> float {
			return this->lastFrameRenderDeltaTime * config::control::kMoveSpeed;
		};

		this->keyboardOnPressRepeatEvents['Q'] = [this, getMoveDistance] { this->modelViewProjection.view.moveUp(getMoveDistance()); };
		this->keyboardOnPressRepeatEvents['E'] = [this, getMoveDistance] { this->modelViewProjection.view.moveDown(getMoveDistance()); };
		this->keyboardOnPressRepeatEvents['W'] = [this, getMoveDistance] { this->modelViewProjection.view.moveFront(getMoveDistance()); };
		this->keyboardOnPressRepeatEvents['A'] = [this, getMoveDistance] { this->modelViewProjection.view.moveLeft(getMoveDistance()); };
		this->keyboardOnPressRepeatEvents['S'] = [this, getMoveDistance] { this->modelViewProjection.view.moveBack(getMoveDistance()); };
		this->keyboardOnPressRepeatEvents['D'] = [this, getMoveDistance] { this->modelViewProjection.view.moveRight(getMoveDistance()); };

		// Mouse.
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Hide coursor.

		const auto& moveLook = [this]() {
			double xPos, yPos;
			glfwGetCursorPos(this->window, &xPos, &yPos);

			xPos -= config::kScreenCenterHorizontal;
			yPos -= config::kScreenCenterVertical;

			// TODO [OPTIMIZATION]
			// Skip to high movement, like entering the window.
			if (abs(xPos) > config::kScreenWidth / 3. || abs(yPos) > config::kScreenHeight / 3.)
				return;

			const float& yaw = float(xPos) * config::control::kMouseSensitivity;
			const float& pitch = -float(yPos) * config::control::kMouseSensitivity;

			this->modelViewProjection.view.moveLook(yaw, pitch);
		};

		this->mouseOnMoveEvents.reserve(10);

		this->mouseOnMoveEvents.push_back({ "UpdateMoveLook", [this, moveLook] { moveLook(); } });
		this->mouseOnMoveEvents.push_back({ "SetCursorCenter", [this] { glfwSetCursorPos(this->window, config::kScreenCenterHorizontal, config::kScreenCenterVertical); } });
	}

	void setCallbacks()
	{
		// glfwSetWindowUserPointer(this->window, reinterpret_cast<void*>(this));
		// glfwSetCursorPosCallback(this->window, control::mouseMoveCallback);
		// glfwSetKeyCallback(this->window, control::keyboardCallback);
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

	void setVertexAttribPointers()
	{
		// TODO [REFACTOR]: Add into seperate class to simplify usage. 
		//		[PERFORMANCE]: Use IndexBuffer.

		size_t idx = 0;

		// Coordinates.
		//const GLint kCoordsIdx = idx++;
		const GLint kCoordsIdx = 0;
		constexpr GLint kAmountOfCoords = 3;
		myGLCall(glEnableVertexAttribArray(kCoordsIdx));
		myGLCall(glVertexAttribPointer(
			kCoordsIdx,                                   // index
			kAmountOfCoords,	                          // size
			GL_FLOAT,			                          // type
			GL_FALSE,			                          // normalized
			sizeof(Vertice3D),		                      // stride
			(void*)(offsetof(Vertice3D, coordinates))));  // offset

		// Texture coordinates.
		//const GLint kTextureCoordsIdx = idx++;
		const GLint kTextureCoordsIdx = 1;
		constexpr GLint kAmountOfTextureCoords = 2;
		myGLCall(glEnableVertexAttribArray(kTextureCoordsIdx));
		myGLCall(glVertexAttribPointer(
			kTextureCoordsIdx,
			kAmountOfTextureCoords,
			GL_FLOAT,
			GL_FALSE,
			sizeof(Vertice3D),
			(void*)(offsetof(Vertice3D, textureCoordinates))));
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

		myGLCall(this->shader.createProgram());
		myGLCall(glUseProgram(this->shader.getShaderProgramID()));

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glBindVertexArray(VAO));

		myGLCall(glGenBuffers(1, &VBO));
		myGLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

		this->setCallbacks();
		this->setEvents();

		myGLCall(glfwSwapInterval(0));

		this->setVertexAttribPointers();

		glfwSetTime(0.0);
		// this->transformersMatrixes = new glm::f32[4 * 16];
	}

	void update()
	{

	}

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

	void addTransformation(const ShapeID& shapeID,
		const transformation3d::TransformatingType& tansformerType,
		const Vector transformationVector)
	{
		using namespace transformation3d;

		switch (tansformerType)
		{
		case TransformatingType::Slide:
			shapesTransformers[shapeID].push_back(std::make_shared<Slider>(Slider(transformationVector)));
			break;
		case TransformatingType::ContinousSlide:
			shapesTransformers[shapeID].push_back(std::make_shared<continous::Slider>(continous::Slider(transformationVector)));
			break;
		case TransformatingType::ContinousScale:
			shapesTransformers[shapeID].push_back(std::make_shared<continous::Scaler>(continous::Scaler(transformationVector)));
			break;
		case TransformatingType::ContinousRotate:
			shapesTransformers[shapeID].push_back(std::make_shared<continous::Rotator>(continous::Rotator(transformationVector)));
			break;
		default:
			throw;
		}
	}

	void setTexture(const std::string& path, ShapeID shapeID)
	{
		this->shapesTextures[shapeID] = std::make_shared<Texture2D>(path);
		const auto& x = this->shapesTextures[shapeID];
	}

	~TheProgram()
	{
		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteBuffers(1, &VBO));
		myGLCall(glDeleteProgram(this->shader.getShaderProgramID()));

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
		//TheProgram* program = static_cast<TheProgram*>(glfwGetWindowUserPointer(window));
	}
};

int main()
{
	TheProgram program;
	program.init();

	const float hexahedronSide = 0.2f;
	const auto& hexahedronID = program.addHexahedron(hexahedronSide, {-hexahedronSide  / 2., -hexahedronSide / 2., hexahedronSide / 2.});
	program.addTransformation(hexahedronID, transformation3d::TransformatingType::ContinousRotate, { 0, 1., 0 });
	program.setTexture("res/textures/arrow_up.jpg", hexahedronID);

	program.mainLoop();
	return 0;
}
