#pragma once

#include <stb_image.h>

#include <string>

class Texture
{
	const std::string path;
	int width = 0;
	int height = 0;
	GLuint textureID = -1;

public:
	GLuint getTextureID() const;
	Texture(const std::string& path);
	void bind(size_t slot);
	void unbind();
	~Texture();
};