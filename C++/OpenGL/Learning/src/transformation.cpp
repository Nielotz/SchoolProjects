#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include "headers/transformation.h"

transformation3d::Slider::Slider(const Vector distanceToSlide)
	:distanceToSlide(distanceToSlide)
{
	this->precalculatedMatrix = glm::translate(
		this->baseOneMatrix,
		glm::vec3(
			this->distanceToSlide.x,
			this->distanceToSlide.y,
			this->distanceToSlide.z
		)
	);
}

const glm::f32* transformation3d::Slider::calculateTransformationMatrix() const
{
	return glm::value_ptr(this->precalculatedMatrix);
}

inline float transformation3d::continous::Transformer::getSinusOfTime() const
{
	return sinf(float(glfwGetTime()));
}

transformation3d::continous::Slider::Slider(const Vector distanceToSlide)
	:distanceToSlide(distanceToSlide)
{}

const glm::f32* transformation3d::continous::Slider::calculateTransformationMatrix() const
{
	const float sinusOfTime = this->getSinusOfTime();
	return glm::value_ptr(
		glm::translate(
			this->baseOneMatrix,
			glm::vec3(
				this->distanceToSlide.x * sinusOfTime,
				this->distanceToSlide.y * sinusOfTime,
				this->distanceToSlide.z * sinusOfTime
			)
		)
	);
}

transformation3d::continous::Scaler::Scaler(const Vector scale)
	:scale(scale)
{}

const glm::f32* transformation3d::continous::Scaler::calculateTransformationMatrix() const
{
	const float sinusOfTime = this->getSinusOfTime();
	return glm::value_ptr(
		glm::scale(
			this->baseOneMatrix,
			glm::vec3(
				scale.x * sinusOfTime,
				scale.y * sinusOfTime,
				scale.z * sinusOfTime
			)
		)
	);
}

transformation3d::continous::Rotator::Rotator(const Vector rotationVector)
	:rotationVector(rotationVector)
{}

const glm::f32* transformation3d::continous::Rotator::calculateTransformationMatrix() const
{
	return glm::value_ptr(
		glm::rotate(glm::mat4(1.0f),
			glm::radians(360.f * this->getSinusOfTime()),
			glm::vec3(rotationVector.x, rotationVector.y, rotationVector.z)
		)
	);
}
