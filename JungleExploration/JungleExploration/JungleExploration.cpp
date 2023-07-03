#include "Starter.hpp"


class JungleExploration : public BaseProject
{
public:
	JungleExploration();

private:
	float Ar;

	void setWindowParameters() {
		// window size, titile and initial background
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Jungle Exploration";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.0f, 0.005f, 0.01f, 1.0f };

		/* TODO */
		uniformBlocksInPool = 9;
		texturesInPool = 7;
		setsInPool = 9;

		Ar = (float)windowWidth / (float)windowHeight;
	}

	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}


	/* TODO */
	void localInit() {}

	/* TODO */
	void pipelinesAndDescriptorSetsInit() {}

	/* TODO */
	void pipelinesAndDescriptorSetsCleanup() {}

	/* TODO */
	void localCleanup() {}

	/* TODO */
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {}

	/* TODO */
	void updateUniformBuffer(uint32_t currentImage) {}
};


int main() {
    JungleExploration app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}