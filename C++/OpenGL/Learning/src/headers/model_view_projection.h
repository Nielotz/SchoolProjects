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
		glm::vec3 getPosition() const;

		void moveUp(const float& moveDistance);
		void moveDown(const float& moveDistance);
		void moveLeft(const float& moveDistance);
		void moveRight(const float& moveDistance);
		void moveFront(const float& moveDistance);
		void moveBack(const float& moveDistance);

		/// <summary>
		/// Update position where camera points.
		/// </summary>
		/// <param name="yaw">Difference in yaw</param>
		/// <param name="pitch">Difference in pitch</param>
		void moveLook(const float& yaw, const float& pitch);

	private:
		// glm::mat4 view_;
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