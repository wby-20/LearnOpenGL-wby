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

class Texture
{
private:
    unsigned int texture;
public:
    Texture(const char* path);
    unsigned int getTexture();
};

Texture::Texture(const char* path)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置纹理参数
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载图片
	int width, height, nrChannals;
	stbi_set_flip_vertically_on_load(true);
	// 主义这里的 STBI_rgh 需要和下边的 GL_RGB 统一
	unsigned char *data = stbi_load(path, &width, &height, &nrChannals, STBI_rgb);
	std::cout << width << " " << height << std::endl;
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
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
    return texture;
}

#endif