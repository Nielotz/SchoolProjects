#pragma once

#include <stb_image.h>

#include <string>

class MyGLTexture
{
	const std::string path;
	int width = 0;
	int height = 0;
	unsigned int textureID = -1;

public:
	unsigned int getTextureID() const;
	MyGLTexture(const std::string& path);
	void bind(size_t slot);
	void unbind();
	~MyGLTexture();
};