#pragma once



struct Vector
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

namespace transformation3d
{
	enum class TransformatingType
	{
		ContinousSlide,
		ContinousRotate,
		ContinousScale
	};

	class Transformer
	{
	public:
		virtual const glm::f32* calculateTransformationMatrix() const = 0;

	protected:
		const glm::mat4& baseOneMatrix = glm::mat4{ 1.0f };

		Transformer() = default;
	};

	namespace continous
	{
		class Transformer : public transformation3d::Transformer
		{
		protected:
			inline float getSinusOfTime() const;
		};

		class Slider : public Transformer
		{
		private:
			const Vector distanceToSlide;

		public:
			Slider(const Vector distanceToSlide);
			const glm::f32* calculateTransformationMatrix() const override;
		};

		class Scaler : public Transformer
		{
			const Vector scale;
		public:
			Scaler(const Vector scale);
			const glm::f32* calculateTransformationMatrix() const override;

		};

		class Rotator : public Transformer
		{
		private:
			const Vector rotationVector;

		public:
			Rotator(const Vector rotationVector);
			const glm::f32* calculateTransformationMatrix() const override;

		};
	}
};