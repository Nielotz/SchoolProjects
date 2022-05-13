#include "headers/config.h"
#include "headers/model_view_projection.h"

#include <GLFW/glfw3.h>
#include "headers/logging/logging.h"


void MVP::View::moveUp(const float& moveDistance)
{
	this->lookingFrom += this->up * moveDistance;
}

void MVP::View::moveDown(const float& moveDistance)
{
	this->lookingFrom -= this->up * moveDistance;
}

void MVP::View::moveLeft(const float& moveDistance)
{
	//logging::info("Moving left", moveDistance);
	this->lookingFrom += glm::normalize(glm::cross(this->lookingFrom, this->up)) * moveDistance;
}

void MVP::View::moveRight(const float& moveDistance)
{
	this->lookingFrom -= glm::normalize(glm::cross(this->lookingFrom, this->up)) * moveDistance;
}

void MVP::View::moveFront(const float& moveDistance)
{
	this->lookingFrom += this->lookingDirection * moveDistance;
}

void MVP::View::moveBack(const float& moveDistance)
{
	this->lookingFrom -= this->lookingDirection * moveDistance;
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
