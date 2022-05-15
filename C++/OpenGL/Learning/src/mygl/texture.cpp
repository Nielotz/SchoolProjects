#include <glad/glad.h>

#include <iostream>

#include "../headers/mygl/texture.h"
#include "../headers/mygl/debug/debug.h"

namespace MyGL
{
	Texture::Texture(const std::string& path)
		: path(path)
	{}

	int Texture::getSlot() const
	{
		return this->slot;
	}

	Texture2D::Texture2D(const std::string& path) : Texture(path)
	{
		stbi_set_flip_vertically_on_load(true);

		int amountOfchannelsInFile;
		const int kAmountOfDesiredChannels = 4;  // RGBA
		auto imageData = stbi_load(path.c_str(),
			&this->width, &this->height,
			&amountOfchannelsInFile, kAmountOfDesiredChannels);

		myGLCall(glGenTextures(1, &this->textureID_GL));  // Generate storage for one texture.
		myGLCall(glBindTexture(GL_TEXTURE_2D, this->textureID_GL));

		myGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		myGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		myGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		myGLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		myGLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData));

		this->bind();

		if (imageData != nullptr)
		    stbi_image_free(imageData);
	}

	GLuint Texture2D::getTextureID() const
	{
		return this->textureID_GL;
	}

	void Texture2D::bind(unsigned int slot)
	{
		this->slot = slot;
		myGLCall(glActiveTexture(GL_TEXTURE0 + slot));
		myGLCall(glBindTexture(GL_TEXTURE_2D, this->textureID_GL));
	}

	void Texture2D::unbind()
	{
		myGLCall(glActiveTexture(GL_TEXTURE0 + this->slot));
		myGLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}

	Texture2D::~Texture2D()
	{
		//myGLCall(glDeleteTextures(1, &this->textureID_GL));
	}

}