#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class MVP
{
private:
	class View
	{
	public:
		glm::mat4 getMatrix() const;

		void moveUp();
		void moveDown();
		void moveLeft();
		void moveRight();
		void moveFront();
		void moveBack();

		void moveLook(const float& yaw, const float& pitch);

	private:
		const float moveSpeed = 0.05f;
		glm::mat4 view_;
		glm::vec3 lookingFrom = { 0.f, 0.f, 1.f };  // Camera position. 
		glm::vec3 lookingDirection = { 0.f, 0.f, -1.f };
		const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f);  // World up.

		float yaw = -90.0f;
		float pitch = 0.f;
	};
public:
	View view;
	glm::mat4 getMVP() const;


	MVP();
private:
	glm::mat4 model_;
	glm::mat4 projection_;
};