#include "../renderer/IoHandler.hpp"
#include "../utils/utils.hpp"  

class GlewGlfwHandler
{
public:
	GlewGlfwHandler(glm::ivec2 windowDimensions, std::string windowName);
	~GlewGlfwHandler() {};
	int init();
	void updateResize();
	
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	GLFWwindow* getWindow();
private:
	GLFWwindow* window;
};