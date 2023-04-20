#include <stb_image.h>
#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <camera.h>
#include <texture.h>

#define WIDTH 1600
#define HEIGHT 920
float weight = 0.5;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float pitch = 0.0f; // 俯仰角
float yaw = -90.0f; // 水平角

Camera camera(cameraPos, cameraUp, yaw, pitch);
// 鼠标上一帧位置
float lastX = WIDTH / 2, lastY = HEIGHT / 2;
bool firstMouse = true;

// 用户改变窗口大小时的回调函数
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// 鼠标的回调
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// 处理用户输入
void process_input(GLFWwindow *window);

int main(int argc, char *argv[])
{
	std::cout << "Hello world!" << std::endl;

	// 初始化glfw
	glfwInit();
	// 告诉glfw openGl版本号,核心模式等
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	// 创建窗口对象
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 将窗口上下文设置为当前线程主上下文
	glfwMakeContextCurrent(window);
	// 初始化GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// 设置窗口维度
	glViewport(0, 0, WIDTH, HEIGHT);
	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	//注册回调函数
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //窗口大小
	glfwSetCursorPosCallback(window, mouse_callback); // 鼠标移动
	glfwSetScrollCallback(window, scroll_callback); // 鼠标滚轮
	// 忽略鼠标指针
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// 创建着色器对象
	Shader objectShader("include/shader/shader.vs", "include/shader/shader.fs");
	Shader lightShader("include/shader/lightShader.vs", "include/shader/lightShader.fs");

	float vertices[] = {
    // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

	// VBO
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 物体VAO
	unsigned int objectVAO;
	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// 法线
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// 纹理坐标
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void* )(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// 灯光VAO
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 光源只有位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	// 加载纹理图片
	Texture diffuseTex("texture/container2.png");
	Texture specularTex("texture/container2_specular.png");
	// Texture emissionTex("texture/matrix.jpg");

	// 循环渲染, 在GLFW退出前一直保持运行
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// 处理用户输入
		process_input(window);

		// 设置清空屏幕的颜色并清空屏幕
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 视角
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		// 物体
		glm::vec3 cubePos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePos);

		// 光照
		float angle = glm::radians(60.0f) * (float)glfwGetTime();
		// float angle = glm::radians(15.0f);
		glm::vec3 lightPos = glm::vec3(2.0f * sin(angle), 1.0f, 2.0f * cos(angle));
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 lightAmbientColor = lightColor;
		glm::vec3 lightDiffuseColor = lightColor;

		// 传输数据
		objectShader.use();
		objectShader.setVec3("viewPos", 1, camera.Position);
		objectShader.setMat4("model", 1, false, model);
		objectShader.setMat4("view", 1, false, view);
		objectShader.setMat4("projection", 1, false, projection);

		// 物体材质
		objectShader.setVec3("material.ambient", 1, glm::vec3(0.24725, 0.1995, 0.2));
		// objectShader.setVec3("material.diffuse", 1, glm::vec3(0.75164, 0.60648, 0.22648));
		objectShader.setInt("material.diffuse", 0);
		// objectShader.setVec3("material.specular", 1, glm::vec3(0.628281, 0.555802, 0.5));
		objectShader.setInt("material.specular", 1);
		objectShader.setFloat("material.shininess", 64.0f);
		
		// 光照材质
		objectShader.setVec3("light.position", 1, lightPos);
		objectShader.setVec3("light.ambient", 1, 0.2f, 0.2f, 0.2f);
		objectShader.setVec3("light.diffuse", 1, 0.5f, 0.5f, 0.5f);
		objectShader.setVec3("light.specular", 1, 1.0f, 1.0f, 1.0f);

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 处理灯光
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));

		lightShader.use();
		lightShader.setMat4("model", 1, false, model);
		lightShader.setMat4("view", 1, false, view);
		lightShader.setMat4("projection", 1, false, projection);

		// 加载纹理
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTex.getTexture());
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularTex.getTexture());

		glBindVertexArray(objectVAO); //绑定VAO
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 交换颜色缓冲, 用来绘制. 并且检测是否有事件发生
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &objectVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	// 释放分配的所有资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void process_input(GLFWwindow *window)
{
	// 按下ESC键时将关闭设置为true
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// 相机操作
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
