#pragma once

#include <stb_image.h>

#include <string>

namespace MyGL
{
	class Texture
	{
	public:
		virtual unsigned int getTextureID() const = 0;

		/// <summary>
		/// Create texture object, and read data from file.
		/// </summary>
		/// <param name="path">path to the file</param>
		virtual void bind(unsigned int slot = 0) = 0;
		virtual void unbind() = 0;
		int getSlot() const;

	protected:
		Texture(const std::string& path);
		const std::string& path;
		int width = -1;
		int height = -1;
		int bpp = -1;
		unsigned int textureID_GL = -1;
		int slot = -1;
	};

	class Texture2D : public Texture
	{
	public:
		unsigned int getTextureID() const;

		Texture2D(const std::string& path);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="slot">preferably keep it under 16 for sure under 32</param>
		void bind(unsigned int slot = 0) override;
		void unbind() override;

		~Texture2D();
	};
}