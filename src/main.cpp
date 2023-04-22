#include <stb_image.h>
#include <iostream>
#include <cmath>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader.h>
#include <camera.h>
#include <texture.h>
#include <mesh.h>
#include <model.h>

#define WIDTH 1600
#define HEIGHT 920

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

float pitch = 0.0f; // 俯仰角
float yaw = -90.0f; // 水平角

vector<Texture> Texture::textureHasLoaded = vector<Texture>();

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

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
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

	Model nano = Model("model/nanosuit.obj");

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

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model,glm::vec3( 0.2f, 0.2f, 0.2f));

		// 传输数据
		objectShader.use();
		objectShader.setVec3("viewPos", camera.Position);
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);
		objectShader.setMat4("model", model);

		objectShader.setFloat("time", (float)glfwGetTime());

		
		// point light 1
        // objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        // objectShader.setVec3("pointLights[0].ambient", 0.005f, 0.005f, 0.005f);
        // objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        // objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        // objectShader.setFloat("pointLights[0].c", 1.0f);
        // objectShader.setFloat("pointLights[0].b", 0.09f);
        // objectShader.setFloat("pointLights[0].a", 0.032f);
        // // point light 2
        // objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        // objectShader.setVec3("pointLights[1].ambient", 0.005f, 0.005f, 0.005f);
        // objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        // objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        // objectShader.setFloat("pointLights[1].c", 1.0f);
        // objectShader.setFloat("pointLights[1].b", 0.09f);
        // objectShader.setFloat("pointLights[1].a", 0.032f);
        

		// 平行光
		objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		objectShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		objectShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		objectShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		// 聚合光
		objectShader.setVec3("spotLight.position", camera.Position);
		objectShader.setVec3("spotLight.direction", camera.Front);
		objectShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		objectShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		objectShader.setFloat("spotLight.otherCutOff", glm::cos(glm::radians(15.0f)));
		objectShader.setFloat("spotLight.c", 1.0f);
        objectShader.setFloat("spotLight.b", 0.09f);
        objectShader.setFloat("spotLight.a", 0.032f);

		nano.Draw(&objectShader);

		// 交换颜色缓冲, 用来绘制. 并且检测是否有事件发生
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	
	// 释放分配的所有资源
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos)
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

void scroll_callback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset)
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
