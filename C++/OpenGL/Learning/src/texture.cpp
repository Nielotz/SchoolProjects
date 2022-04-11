#include <glad/glad.h>

#include <iostream>

#include "headers/texture.h"

Texture::Texture(const std::string& path)
	:path(path)
{
	stbi_set_flip_vertically_on_load(true);

	int channelsInFile;
	auto imageData = stbi_load(path.c_str(), &this->width, &this->height, &channelsInFile, 3);

	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (imageData != nullptr)
		stbi_image_free(imageData);
}

GLuint Texture::getTextureID() const
{
	return this->textureID;
}

void Texture::bind(size_t slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	this->unbind();
	glDeleteTextures(1, &this->textureID);
}
