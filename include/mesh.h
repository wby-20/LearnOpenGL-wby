#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <texture.h>

#include <vector>
#include <string>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    void Draw(Shader shader);
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) : vertices(vertices), indices(indices), textures(textures)
    {
        setupMesh();
    }
private:
    unsigned int VAO, VBO, EBO;
    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // 配置数据缓存
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
        // 配置索引缓存
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        // 位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
        glEnableVertexAttribArray(0);
        // 法线
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);
        // 纹理坐标
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void* )offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        // 消除依赖
        glBindVertexArray(0);
    }
};

void Mesh::Draw(Shader shader)
{
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;
    for(int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        string name = textures[i].getType();
        string number;

        if(name == "texture_diffuse")
            number = to_string(diffuseNum++);
        else if(name == "texture_specular")
            number = to_string(specularNum++);

        shader.setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].getTexture());
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

#endif
