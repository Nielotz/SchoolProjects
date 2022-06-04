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
	struct Scene
	{
		color::RGB ambientLightColor;
		float ambientLightStrength;
	};

	struct Lightning
	{
		bool isAmbientLight = 1;
		bool isDiffuseLight = 1;
		bool isSpecularLight = 1;
	};

	GLFWwindow* window = nullptr;

	typedef uint64_t ShapeID;
	typedef drawable::shape3d::Shape Shape3D;

	// TODO [PERFORMANCE]: Group shapes stuff (transformations, textures, etc.) in one place.

	// Shapes.
	std::unordered_map<ShapeID, std::shared_ptr<Shape3D>> shapes;

	// Transformations.
	typedef std::vector<std::shared_ptr<transformation3d::Transformer>> VectorOfTransformers;
	std::unordered_map<ShapeID, VectorOfTransformers> shapesTransformers;

	// Textures.
	// TODO [REFACTOR]: Add manager or simplify structure.
	typedef MyGL::Texture2D Texture2D;
	typedef uint64_t TextureID;
	std::unordered_map<TextureID, std::shared_ptr<Texture2D>> textures;
	std::unordered_map<ShapeID, TextureID> shapesTextures;
	// Stores information whether slot is in use.
	bool usedTextureSlots[32]{ false };

	// TODO [REFACTOR]: Generialize.
	// Light sources.
	typedef uint64_t LightSourceID;
	typedef drawable::lighting3d::LightSourceHexahedron LightSourceHexahedron;
	std::unordered_map<LightSourceID, std::shared_ptr<LightSourceHexahedron>> lightSources;

	// Events.
	// TODO: merge it into one with enum fron event
	typedef int KeyID;
	std::unordered_map<KeyID, std::function<void()>> keyboardOnPressRepeatEvents;
	std::unordered_map<KeyID, std::function<void()>> keyboardOnPressEvents;
	std::vector<std::pair<std::string, std::function<void()>>> mouseOnMoveEvents;

	// TODO [REFACTOR]: Find a home for shaders.
	// Shaders.
	MyGLShader cubeShader;
	MyGLShader lightShader;

	// TODO [REFACTOR]: Move it to VertexArrayManager / VAO Manager.
	// TODO [PERFORMANCE]: Swap to index buffers.
	unsigned int VBO = -1, VAO = -1;
	unsigned int lightVBO = -1, lightVAO = -1;

	MVP modelViewProjection;

	Scene scene;
	Lightning lightning;

	// In seconds.
	float lastFrameRenderDeltaTime = 0.f;
	// Time from glfwGetTime() received after render of a frame. In seconds.
	float lastStartFrameRenderTime = 0.f;

	typedef drawable::primitive::Point3D Point3D;
	typedef drawable::primitive::Vertice3D Vertice3D;

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
		Vertice3D* allVertices = nullptr;

		struct ShapeToDraw
		{
			const ShapeID shapeID;
			const std::shared_ptr<Shape3D> shape;
			const std::vector<Vertice3D> vertices;
		};

		// TODO [PERFORMANCE]: Cache shapes and update in update().
		std::vector<ShapeToDraw> shapesToDraw;

		template <typename T>
		ShapesToDraw(const std::unordered_map<ShapeID, std::shared_ptr<T>>& shapes)
		{
			ASSERT(typeid(T) == typeid(Shape3D)
				|| typeid(T) == typeid(LightSourceHexahedron));

			shapesToDraw.reserve(shapes.size());

			// Fill shapesToDraw and count total amount of vertices.
			for (const auto& [shapeID, shape] : shapes)
			{
				const std::vector<Vertice3D>& shapeVertices = shape->getVertices();

				totalAmountOfVertices += shapeVertices.size();

				shapesToDraw.push_back(ShapeToDraw{ shapeID, shape, shapeVertices });
			}

			// Allocate data and copy vertices data into one blob.
			// TODO [PERFORMANCE]: Cache vertices.
			this->allVertices = new Vertice3D[this->totalAmountOfVertices];

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


	// Perfaps maybe dont ya think about refactoring THIS? TODO [REFACTOR]: Seperate it to a class.
	// 1. Copy shapes data to the GPU buffer. TODO [OPTIMIZATION]: Do it on update().
	// 2. Draw all shapes: one shape at the time.
	void draw()
	{
		/// Prepare data to draw.

		// TODO [REFACTOR]: Find better place for it.
		// Draw polygons.
		const auto& drawShapes = [this](
			const std::unordered_map<ShapeID, std::shared_ptr<Shape3D>>& shapes,
			const std::shared_ptr<LightSourceHexahedron>& lightSource) {
				/// Prepare polygons.
				ShapesToDraw shapesToDraw(shapes);

				/// Prepare data.
				// _TODO [PERFORMANCE]: Move to update.
				myGLCall(glBindVertexArray(this->VAO));
				myGLCall(glBindBuffer(GL_ARRAY_BUFFER, this->VBO));
				this->setVertexAttribPointers();
				myGLCall(glUseProgram(this->cubeShader.getShaderProgramID()));

				myGLCall(glBufferData(GL_ARRAY_BUFFER,
					sizeof(Vertice3D) * shapesToDraw.totalAmountOfVertices,
					static_cast<const void*>(shapesToDraw.allVertices),
					GL_STATIC_DRAW
				));

				this->cubeShader.setGLlUniform1i("u_isAmbientLight", this->lightning.isAmbientLight);
				this->cubeShader.setGLlUniform1i("u_isDiffuseLight", this->lightning.isDiffuseLight);
				this->cubeShader.setGLlUniform1i("u_isSpecularLight", this->lightning.isSpecularLight);

				this->cubeShader.setGLUniform3f("u_viewPos", this->modelViewProjection.view.getPosition());

				this->cubeShader.setGLUniform1f("u_ambientLightStrength", this->scene.ambientLightStrength);
				this->cubeShader.setGLUniform3f("u_ambientLightColor", this->scene.ambientLightColor);

				const auto& lightSourceCenter = lightSource->getCenter();
				glm::vec4 lightSourcePosition(lightSourceCenter.x, lightSourceCenter.y, lightSourceCenter.z, 1.);

				glm::mat4 transformationMatrix(1.f);
				for (const auto& [shapeID, transformers] : this->shapesTransformers)
					if (this->lightSources.contains(shapeID))
					{
						for (const auto& transformer : transformers)
							transformationMatrix *= transformer->calculateTransformationMatrix();
						break;
					}
				lightSourcePosition = transformationMatrix * lightSourcePosition;

				// std::cout << "x: " << lightSourcePosition.x << " y: " << lightSourcePosition.y << " z: " << lightSourcePosition.z << std::endl;
				this->cubeShader.setGLUniform3f("u_lightPosition", lightSourcePosition.x, lightSourcePosition.y, lightSourcePosition.z);

				const auto& color = lightSource->getColor();
				this->cubeShader.setGLUniform3f("u_lightColor", color.red, color.green, color.blue);

				this->cubeShader.setGLlUniformMat4f("u_MVP", this->modelViewProjection.getMVP());

				// Actual drawing. 
				GLint verticesOffset = 0;
				for (const auto& [shapeID, shape, vertices] : shapesToDraw.shapesToDraw)
				{
					// Textures.
					// TODO [FUNCTIONAL]: Add default texture.
					ASSERT(this->shapesTextures.contains(shapeID));
					const auto& shapeTextureID = this->shapesTextures.at(shapeID);
					ASSERT(this->textures.contains(shapeTextureID));
					this->cubeShader.setGLlUniform1i("u_texture", this->textures.at(shapeTextureID)->getSlot());

					transformationMatrix = glm::mat4(1.);
					// Merge transformations.
					if (this->shapesTransformers.contains(shapeID))
						for (const auto& transformer : this->shapesTransformers.at(shapeID))
							transformationMatrix *= transformer->calculateTransformationMatrix();

					this->cubeShader.setGLlUniformMat4f("u_transformation", transformationMatrix);

					this->cubeShader.setGLUniform1f("u_lightSpecularStrength", shape->getSpecularStrength());

					// Draw triangle by triangle.
					constexpr int kAmountOfVerticesInTriangles = 3;
					for (auto i = 0; i < vertices.size(); i += kAmountOfVerticesInTriangles)
					{
						/// Calculate normal.
						auto coordinates1 = vertices[i].coordinates;
						auto coordinates2 = vertices[i + 1].coordinates;
						auto coordinates3 = vertices[i + 2].coordinates;

						glm::vec3 point1(coordinates1.x, coordinates1.y, coordinates1.z);
						glm::vec3 point2(coordinates2.x, coordinates2.y, coordinates2.z);
						glm::vec3 point3(coordinates3.x, coordinates3.y, coordinates3.z);

						point1 = transformationMatrix * glm::vec4(point1, 1.);
						point2 = transformationMatrix * glm::vec4(point2, 1.);
						point3 = transformationMatrix * glm::vec4(point3, 1.);

						glm::vec3 vector1 = point2 - point1;
						glm::vec3 vector2 = point3 - point1;

						glm::vec3 normal = glm::normalize(glm::cross(vector1, vector2));

						// std::cout << "x: " << normal.x << " y: " << normal.y << " z: " << normal.z << std::endl;

						this->cubeShader.setGLUniform3f("u_normal", normal.x, normal.y, normal.z);

						myGLCall(glDrawArrays(GL_TRIANGLES,
							(GLint)verticesOffset,
							static_cast<GLsizei>(kAmountOfVerticesInTriangles)
						));
						verticesOffset += kAmountOfVerticesInTriangles;
					}
				}
		};

		const auto& drawLightning = [this](
			const std::unordered_map<ShapeID, std::shared_ptr<LightSourceHexahedron>>& lightShapes) {
				// Prepare light.
				ShapesToDraw lightSourcesToDraw(this->lightSources);

				/// Prepare data.
				// _TODO [PERFORMANCE]: Move to update.
				myGLCall(glBindVertexArray(this->lightVAO));
				myGLCall(glBindBuffer(GL_ARRAY_BUFFER, this->lightVBO));
				this->setVertexAttribPointersLightning();
				myGLCall(glUseProgram(this->lightShader.getShaderProgramID()));

				myGLCall(glBufferData(GL_ARRAY_BUFFER,
					sizeof(Vertice3D) * lightSourcesToDraw.totalAmountOfVertices,
					static_cast<const void*>(lightSourcesToDraw.allVertices),
					GL_STATIC_DRAW
				));

				this->lightShader.setGLlUniformMat4f("u_MVP", this->modelViewProjection.getMVP());

				// Actual drawing. 
				GLint verticesOffset = 0;
				for (const auto& [shapeID, shape, vertices] : lightSourcesToDraw.shapesToDraw)
				{
					/// Merge transformations.
					glm::mat4 transformationMatrix(1.f);

					if (this->shapesTransformers.contains(shapeID))
						for (const auto& transformer : this->shapesTransformers.at(shapeID))
							transformationMatrix *= transformer->calculateTransformationMatrix();

					this->lightShader.setGLlUniformMat4f("u_transformation", transformationMatrix);
					this->lightShader.setGLUniform4f("u_color", shape->getColor());

					/// Draw triangle by triangle.
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

		for (const auto& [key, lightSource] : this->lightSources)
		{
			drawShapes(this->shapes, lightSource);
			break;
		}

		drawLightning(this->lightSources);

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

	void setVertexAttribPointersLightning()
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
	void toggleAmbientLightning()
	{
		this->lightning.isAmbientLight = !this->lightning.isAmbientLight;
	}
	void toggleDiffuseLightning()
	{
		this->lightning.isDiffuseLight = !this->lightning.isDiffuseLight;
	}
	void toggleSpecularLightning()
	{
		this->lightning.isSpecularLight = !this->lightning.isSpecularLight;
	}

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

		this->cubeShader.createProgramFromFile("src/mygl/shader/7.1.lighting.cubes.shader");
		this->lightShader.createProgramFromFile("src/mygl/shader/7.1.lighting.light_sources.shader");

		myGLCall(glGenVertexArrays(1, &VAO));
		myGLCall(glGenBuffers(1, &VBO));

		myGLCall(glGenVertexArrays(1, &lightVAO));
		myGLCall(glGenBuffers(1, &lightVBO));

		this->setCallbacks();
		this->setEvents();

		myGLCall(glfwSwapInterval(2));

		glfwSetTime(0.0);
	}

	ShapeID addHexahedron(float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA)
	{
		// TODO [PERFORAMNCE]: Swap to random id.
		// Find first avaiable id.
		ShapeID shapeID = 0;
		while (this->shapes.contains(shapeID) || this->lightSources.contains(shapeID))
			shapeID++;

		this->shapes[shapeID] = std::make_unique<drawable::shape3d::Hexahedron>(sideLength, position, color);

		return shapeID;
	}

	LightSourceID addLightSourceHexahedron(
		float sideLength, Point3D position = { 0.f, 0.f, 0.f }, color::RGBA color = color::kRedRGBA,
		float luminosity = 1.)
	{
		// TODO [PERFORAMNCE]: Swap to random id.
		// Find first avaiable id.
		LightSourceID lightSourceID = 0;
		while (this->shapes.contains(lightSourceID) || this->lightSources.contains(lightSourceID))
			lightSourceID++;

		this->lightSources[lightSourceID] =
			std::make_unique<LightSourceHexahedron>(sideLength, position, color, luminosity);

		return lightSourceID;
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
		case TransformatingType::Scale:
			shapesTransformers[shapeID].push_back(std::make_shared<Scaler>(Scaler(transformationVector)));
			break;
		default:
			throw;
		}
	}

	ShapeID addShapeFromOBJFile(const std::string& path, const float& scale = 1)
	{
		// TODO [PERFORAMNCE]: Swap to random id.
		// Find first avaiable id.
		ShapeID shapeID = 0;
		while (this->shapes.contains(shapeID) || this->lightSources.contains(shapeID))
			shapeID++;

		this->shapes[shapeID] = obj_loader::loadFromFile(path);

		return shapeID;
	}

	// TODO [PERFORAMNCE]: Cache.
	// TODO [REFACTOR]: Rewrite it better.
	void assignTexture(const ShapeID& shapeID, const TextureID& textureID)
	{
		this->shapesTextures[shapeID] = textureID;

		// TODO [PERFORAMNCE]: .
		// Find first avaiable slotIdx.
		int slotIdx = 0;
		while (this->usedTextureSlots[slotIdx])
		{
			ASSERT(slotIdx < 32);
			slotIdx++;
		}

		this->textures[textureID]->bind(slotIdx);
		this->usedTextureSlots[slotIdx] = true;
	}

	TextureID loadTexture(const std::string& path)
	{
		// TODO [PERFORMANCE] [REFACTOR]
		// Find first avaiable textureID.
		TextureID textureID = 0;
		while (this->textures.contains(textureID))
			textureID++;

		this->textures[textureID] = std::make_shared<Texture2D>(path);

		return textureID;
	}

	void setScene(color::RGB ambientLightColor, float ambientLightStrength)
	{
		this->scene.ambientLightColor = ambientLightColor;
		this->scene.ambientLightStrength = ambientLightStrength;
	}

	~TheProgram()
	{
		myGLCall(glDeleteVertexArrays(1, &VAO));
		myGLCall(glDeleteVertexArrays(1, &lightVAO));
		myGLCall(glDeleteBuffers(1, &VBO));
		myGLCall(glDeleteProgram(this->cubeShader.getShaderProgramID()));
		myGLCall(glDeleteProgram(this->lightShader.getShaderProgramID()));

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
			if (key == GLFW_KEY_I)
				program->toggleAmbientLightning();
			else if (key == GLFW_KEY_O)
				program->toggleDiffuseLightning();
			else if (key == GLFW_KEY_P)
				program->toggleSpecularLightning();
		}
	}
};

int main()
{
	TheProgram program;
	program.init();

	// Transformation type.
	const auto& ContinousRotate3D = transformation3d::TransformatingType::ContinousRotate;
	const auto& ContinousSlide3D = transformation3d::TransformatingType::ContinousSlide;
	const auto& Scale3D = transformation3d::TransformatingType::Scale;

	// Textures.
	const auto& textureArrowUPID = program.loadTexture("res/textures/arrow_up.jpg");

	// Scene.
	program.setScene({ 1.f, 1.f, 1.f }, 0.1f);

	// The Cube.
	const float hexahedronSide = 0.2f;
	// const auto& hexahedronID = program.addShapeFromOBJFile("res/obj_files/sample_obj.txt");
	// const auto& hexahedronID = program.addHexahedron(hexahedronSide,
	// 	{ -hexahedronSide / 2., -hexahedronSide / 2., hexahedronSide / 2. }
	// );
	// program.addTransformation(hexahedronID, ContinousRotate3D, { 0, 1., 0 });
	// program.assignTexture(textureArrowUPID, hexahedronID);

	// Light source.
	const float lightSourceHexahedronSide = 0.002f;
	const float lightSourceLuminosity = 1.0f;
	const auto& lightSourceHexahedronID = program.addLightSourceHexahedron(
		lightSourceHexahedronSide,
		{ -lightSourceHexahedronSide / 2. , -lightSourceHexahedronSide / 2., lightSourceHexahedronSide * 500 },
		color::kWhiteRGBA,
		lightSourceLuminosity);  // Luminosity.
	program.addTransformation(lightSourceHexahedronID, ContinousSlide3D, { 0, 1.f, 0 });

	const auto& lisekID = program.addShapeFromOBJFile("res/obj_files/lisek.obj");
	program.assignTexture(lisekID, textureArrowUPID);
	program.addTransformation(lisekID, Scale3D, { 0.01f, 0.01f, 0.01f });
	program.addTransformation(lisekID, ContinousRotate3D, { 0.f, 1.f, 0.f });

	program.mainLoop();
	return 0;
}
