/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <ionicengine/ionicengine.h>
#include <ionicengine/graphics/screen.h>
#include <lambdacommon/system/system.h>
#include "learnopengl/camera.h"

using namespace lambdacommon;
using namespace ionicengine;

// settings
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
double lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos{1.2f, 1.0f, 2.0f};

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

Font *textFont;

class CubesLightningScreen : public Screen
{
private:
	unsigned int cubeVAO, lightVAO, vbo;
	Window *window;

public:
	CubesLightningScreen(Window *window, float *vertices) : window(window)
	{
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cubeVAO);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
		glEnableVertexAttribArray(0);

		// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		// we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
		glEnableVertexAttribArray(0);

		setBackgroundColor({.1, .1, .1});
	}

	~CubesLightningScreen()
	{
		glDeleteVertexArrays(1, &cubeVAO);
		glDeleteVertexArrays(1, &lightVAO);
		glDeleteBuffers(1, &vbo);
	}

	void draw(Graphics *graphics) override
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window->getHandle());

		auto lightningShader = shader::getShader({"cubes_lightning:shaders/colors"});
		lightningShader.use();
		lightningShader.setColor("objectColor", {1.f, .5f, .31f});
		lightningShader.setColor("lightColor", Color::COLOR_WHITE);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(graphics->getWidth()) /
																	static_cast<float>(graphics->getHeight()),
												.1f, 100.f);
		glm::mat4 view = camera.GetViewMatrix();

		lightningShader.setMatrix4f("projection", projection);
		lightningShader.setMatrix4f("view", view);

		glm::mat4 model;
		lightningShader.setMatrix4f("model", model);

		// Render the cube.
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Draw the lamp object.
		auto lampShader = shader::getShader({"cubes_lightning:shaders/lamp"});
		lampShader.use();
		lampShader.setMatrix4f("projection", projection);
		lampShader.setMatrix4f("view", view);
		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(.2f));
		lampShader.setMatrix4f("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawText(*textFont, graphics->getWidth() / 2.f, graphics->getHeight() / 2.f, "X");
	}

	void update() override
	{

	}
};

class DebugOverlay : public Overlay
{
private:
	Font font;
public:
	DebugOverlay(const Font &font) : font(font)
	{}

	void draw(Graphics *graphics) override
	{
		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawText(font, 2, 3 + font.getHeight(), "Position: X:" + std::to_string(camera.Position.x) + " Y:" + std::to_string(camera.Position.y) + " Z:" + std::to_string(camera.Position.z) + "\n"
		 + "Yaw: " + std::to_string(camera.Yaw) + ", Pitch: " + std::to_string(camera.Pitch) + "\nZoom: " + std::to_string(camera.Zoom));
	}

	void update() override
	{

	}
};

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = static_cast<float>(xpos);
		lastY = static_cast<float>(ypos);
		firstMouse = false;
	}

	float xoffset = static_cast<float>(xpos - lastX);
	float yoffset = static_cast<float>(lastY - ypos); // reversed since y-coordinates go from bottom to top

	lastX = static_cast<float>(xpos);
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main()
{
	terminal::setup();

	ionicengine::init(true);

	ScreenManager screenManager{};

	auto windowOptions = ionicengine::DEFAULT_WINDOW_OPTIONS;
	windowOptions.context_version_major = 3;
	windowOptions.context_version_minor = 3;
	windowOptions.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
	windowOptions.samples = 4;
	auto window = window::createWindow("IonicEngine - Cubes Lightning", WIDTH, HEIGHT, windowOptions);
	window.requestContext();
	glfwSetCursorPosCallback(window.getHandle(), mouse_callback);
	glfwSetScrollCallback(window.getHandle(), scroll_callback);
	// tell GLFW to capture our mouse
	glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		ionicengine::shutdown();
		glfwTerminate();
		return err;
	}

	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_DEPTH_TEST);

	auto font = ionicengine::getFontManager().loadFont(std::string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	textFont = &font.value();

	OverlayFPS overlay{font.value()};
	screenManager.registerOverlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screenManager.addActiveOverlay(IONICENGINE_OVERLAYS_FPS);

	DebugOverlay debugOverlay{font.value()};
	ResourceName debugResource{"cubes_lightning:overlays/debug"};
	screenManager.registerOverlay(debugResource, &debugOverlay);
	screenManager.addActiveOverlay(debugResource);

	getGraphicsManager()->init();

	shader::compile({"cubes_lightning:shaders/colors"});
	shader::compile({"cubes_lightning:shaders/lamp"});

	ResourceName cubesLightningScreen{"ionicengine:screens/cubes_lightning"};

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
	};
	CubesLightningScreen screen{&window, vertices};

	screenManager.registerScreen(cubesLightningScreen, &screen);
	screenManager.setActiveScreen(cubesLightningScreen);

	screenManager.attachWindow(window);

	screenManager.startLoop();

	/*while (!window.shouldClose())
	{
		screenManager.render();

		glfwSwapBuffers(window.getHandle());
		glfwPollEvents();
	}*/

	ionicengine::shutdown();
	return EXIT_SUCCESS;
}
