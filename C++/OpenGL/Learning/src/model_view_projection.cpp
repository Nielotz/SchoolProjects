#include "headers/config.h"
#include "headers/model_view_projection.h"

#include <GLFW/glfw3.h>
#include "headers/logging/logging.h"


void MVP::View::moveUp()
{
	this->lookingFrom += this->up * moveSpeed;
}

void MVP::View::moveDown()
{
	this->lookingFrom -= this->up * moveSpeed;
}

void MVP::View::moveLeft()
{
	this->lookingFrom += glm::normalize(glm::cross(this->lookingFrom, this->up)) * moveSpeed;
}

void MVP::View::moveRight()
{
	this->lookingFrom -= glm::normalize(glm::cross(this->lookingFrom, this->up)) * moveSpeed;
}

void MVP::View::moveFront()
{
	this->lookingFrom += moveSpeed * this->lookingDirection;
}

void MVP::View::moveBack()
{
	this->lookingFrom -= moveSpeed * this->lookingDirection;
}

void MVP::View::moveLook(const float& yaw, const float& pitch)
{
	this->yaw += yaw;
	this->pitch += pitch;

	// TODO [OPTIMIZATION]
	if (this->pitch > 89.0f)
		this->pitch = 89.0f;
	if (this->pitch < -89.0f)
		this->pitch = -89.0f;

	this->lookingDirection = glm::normalize(glm::vec3(
			cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch)),  // x
			sin(glm::radians(this->pitch)),							// y 
			sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch))   // z
		));
}

glm::mat4 MVP::View::getMatrix() const
{
	return glm::lookAt(
		this->lookingFrom,
		this->lookingFrom + this->lookingDirection,
		this->up);
}

glm::mat4 MVP::getMVP() const
{
	return this->projection_ * this->view.getMatrix() * this->model_;
}

MVP::MVP()
{
	this->model_ = glm::mat4(1.f);
	this->projection_ = glm::perspective(
		glm::radians(45.0f), 
		float(config::kScreenWidth) / float(config::kScreenHeight), 
		0.1f, 100.0f);
}
