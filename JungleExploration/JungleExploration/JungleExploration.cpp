#include "Starter.hpp"


class JungleExploration : public BaseProject 
{
protected:
	float Ar;

	void setWindowParameters() 
	{
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Jungle Exploration";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.2f, 0.01f, 1.0f};
		
		Ar = (float)windowWidth / (float)windowHeight;
	}
	
	void onWindowResize(int w, int h) 
	{
		Ar = (float)w / (float)h;
	}
	
	void localInit() 
	{
		/* TODO */
	}
	
	void pipelinesAndDescriptorSetsInit() 
	{
		/* TODO */
	}

	void pipelinesAndDescriptorSetsCleanup() 
	{
		/* TODO */
	}
	
	void localCleanup() 
	{
		/* TODO */
	}
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) 
	{
		/* TODO */
	}

	void updateUniformBuffer(uint32_t currentImage) 
	{
		/* TODO */
	}	
};


// This is the main: probably you do not need to touch this!
int main() {
	JungleExploration app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}