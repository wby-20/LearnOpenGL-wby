#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <string>
#include <iostream>
#include <vector>

using namespace std;

enum textureType
{
	typeDiffuce,
	typeSpecular
};

class Texture
{
private:
    
public:
	static vector<Texture> textureHasLoaded;

	unsigned int texture;
	textureType type;
	string path;

    Texture(const char* path, textureType type, string directory);
    unsigned int getTexture();
	textureType getType();
};

Texture::Texture(const char* path, textureType type, string directory) : type(type), path(directory)
{
    glGenTextures(1, &texture);
    // 加载图片
	int width, height, nrChannals;
	// stbi_set_flip_vertically_on_load(true);
	// 注意这里的 STBI_rgb 需要和下边的 GL_RGB 统一
	unsigned char *data = stbi_load(path, &width, &height, &nrChannals, STBI_rgb);
	std::cout << width << " " << height << std::endl;
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		// 设置纹理参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	// 释放图像内存
	stbi_image_free(data);
}

unsigned int Texture::getTexture()
{
    return this->texture;
}

textureType Texture::getType()
{
	return this->type;
}

#endif