#include "Starter.hpp"


struct UniformBufferObject {
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct GlobalUniformBufferObject 
{
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct VertexMesh
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
};

//void GameLogic(JungleExploration* A, float Ar, glm::mat4& ViewPrj, glm::mat4& World);

class JungleExploration : public BaseProject 
{
protected:
	// Descriptor Set Layouts
	DescriptorSetLayout DSLGubo, DSLToon, DSLToonBlinn;

	// Vertex formats
	VertexDescriptor VMesh;

	// Pipelines
	Pipeline PToon, PToonBlinn;

	// Models
	Model<VertexMesh> MCharacter, MGround;

	// Textures
	Texture TCharacter, TGround;

	// Descriptor sets
	DescriptorSet DSGubo, DSCharacter, DSGround;

	// Uniform Blocks
	UniformBufferObject uboCharacter, uboGround;
	GlobalUniformBufferObject gubo;

	// Other Parameters
	float Ar;
	float CamH, CamRadius, CamPitch, CamYaw;

	void setWindowParameters() 
	{
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Jungle Exploration";
    	windowResizable = GLFW_TRUE;
		initialBackgroundColor = {0.0f, 0.0f, 0.01f, 1.0f};
		
		Ar = (float)windowWidth / (float)windowHeight;
	}

	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}
	
	void setDescriptorPool()
	{
		uniformBlocksInPool = 10;
		texturesInPool = 10;
		setsInPool = 10;
	}

	void localInit() 
	{
		/* TODO */
		// Initializing Descriptor Set Layouts
		DSLToon.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLToonBlinn.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLGubo.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});

		// Initializing Vertex Descriptor
		VMesh.init(this, {
			{0, sizeof(VertexMesh), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
				{0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, pos),
					   sizeof(glm::vec3), POSITION},
				{0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(VertexMesh, norm),
					   sizeof(glm::vec3), NORMAL},
				{0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexMesh, uv),
					   sizeof(glm::vec2), UV}
			});

		// Initializing Pipelines
		PToon.init(this, &VMesh, "shaders/ToonVert.spv", "shaders/ToonFrag.spv", { &DSLGubo, &DSLToon });
		PToonBlinn.init(this, &VMesh, "shaders/ToonBlinnVert.spv", "shaders/ToonBlinnFrag.spv", { &DSLGubo, &DSLToonBlinn });

		// Initializing Models
		MCharacter.init(this, &VMesh, "Models/character.obj", OBJ);
		MGround.init(this, &VMesh, "Models/ground.obj", OBJ);

		// Initializing Textures
		TCharacter.init(this, "textures/Wood.png");
		TGround.init(this, "textures/Grass.png");

		// Init local variables
		CamH = 1.0f;
		CamRadius = 3.0f;
		CamPitch = glm::radians(15.f);
		CamYaw = glm::radians(30.f);
	}
	
	void pipelinesAndDescriptorSetsInit() 
	{
		/* TODO */
		// Creating Pipelines
		PToon.create();
		PToonBlinn.create();

		// Defining the Descriptor Sets
		DSCharacter.init(this, &DSLToon, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &TCharacter}
			});
		DSGround.init(this, &DSLToonBlinn, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TGround}
			});
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});
	}

	void pipelinesAndDescriptorSetsCleanup() 
	{
		/* TODO */
		// Cleanup Pipelines
		PToon.cleanup();
		PToonBlinn.cleanup();

		// Cleanup Descriptor Sets
		DSCharacter.cleanup();
		DSGround.cleanup();
		DSGubo.cleanup();
	}
	
	void localCleanup() 
	{
		/* TODO */
		// Cleanup Textures
		TCharacter.cleanup();
		TGround.cleanup();

		// Cleanup Models
		MCharacter.cleanup();
		MGround.cleanup();

		// Cleanup Descriptor Set Layouts
		DSLGubo.cleanup();
		DSLToon.cleanup();
		DSLToonBlinn.cleanup();

		// Destroying the Pipeline
		PToon.destroy();
		PToonBlinn.destroy();
	}
	
	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) 
	{
		/* TODO */
		// Set Gubo
		DSGubo.bind(commandBuffer, PToon, 0, currentImage);

		// Character
		// Binding the Pipeline
		PToon.bind(commandBuffer);
		// Binding the Model
		MCharacter.bind(commandBuffer);
		// Binding the Descriptor Set
		DSCharacter.bind(commandBuffer, PToon, 1, currentImage);
		// Record the drawing command in the command buffer
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MCharacter.indices.size()), 1, 0, 0, 0);


		// Ground
		PToonBlinn.bind(commandBuffer);
		MGround.bind(commandBuffer);
		DSGround.bind(commandBuffer, PToonBlinn, 1, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MGround.indices.size()), 1, 0, 0, 0);
	}

	void updateUniformBuffer(uint32_t currentImage) 
	{
		/* TODO */
		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		// Integration with the timers and the controllers
		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		// For Getting the inputs
		getSixAxis(deltaT, m, r, fire);
		glm::mat4 ViewPrj;
		glm::mat4 World;
		
		GameLogic(this, Ar, ViewPrj, World);



		gubo.DlightDir = glm::normalize(glm::vec3(1, 2, 3));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = glm::vec3(100.0, 100.0, 100.0);

		// Writes value to the GPU
		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		//glm::mat4 World = glm::mat4(1);
		uboCharacter.amb = 1.0f; uboCharacter.gamma = 180.0f; uboCharacter.sColor = glm::vec3(1.0f);
		uboCharacter.mvpMat = ViewPrj * World;
		uboCharacter.mMat = World;
		uboCharacter.nMat = glm::inverse(glm::transpose(World));
		DSCharacter.map(currentImage, &uboCharacter, sizeof(uboCharacter), 0);

		//World = glm::scale(glm::mat4(1),glm::vec3(10));
		uboGround.amb = 1.0f; uboGround.gamma = 180.0f; uboGround.sColor = glm::vec3(1.0f);
		uboGround.mvpMat = ViewPrj * World;
		uboGround.mMat = World;
		uboGround.nMat = glm::inverse(glm::transpose(World));
		DSGround.map(currentImage, &uboGround, sizeof(uboGround), 0);
	}	



	float yaw = 0, pitch = 0, roll = 0;
	glm::quat rot = glm::quat(1, 0, 0, 0);

	glm::mat4 ViewPrjOld = glm::mat4(1);


	void GameLogic(JungleExploration* JE, float Ar, glm::mat4& ViewPrj, glm::mat4& World)
	{
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;

		// Player starting point
		const glm::vec3 StartingPosition = glm::vec3(-41.50, 0.0, -19.5);

		// Camera target height and distance
		const float camHeight = 0.25;
		const float camDist = 1.5;
		// Camera Pitch limits
		const float minPitch = glm::radians(-8.75f);
		const float maxPitch = glm::radians(60.0f);
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 2.0f;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		JE->getSixAxis(deltaT, m, r, fire);

		static glm::vec3 Pos = StartingPosition;

		glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1));
		glm::vec3 uy = glm::vec3(0, 1, 0);
		glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1));
		pitch += ROT_SPEED * r.x * deltaT;
		yaw += ROT_SPEED * r.y * deltaT;
		roll += ROT_SPEED * r.z * deltaT;
		Pos += ux * MOVE_SPEED * m.x * deltaT;
		Pos += uy * MOVE_SPEED * m.y * deltaT;
		Pos += uz * MOVE_SPEED * m.z * deltaT;
		glm::mat4 T = glm::translate(glm::mat4(1.0), Pos);
		if (pitch <= minPitch)
			pitch = minPitch;
		else if (pitch >= maxPitch)
			pitch = maxPitch;
		glm::mat4 Rx = glm::rotate(glm::mat4(1.0), pitch, glm::vec3(1, 0, 0));
		glm::mat4 Ry = glm::rotate(glm::mat4(1.0), yaw, glm::vec3(0, 1, 0));
		glm::mat4 Rz = glm::rotate(glm::mat4(1.0), roll, glm::vec3(0, 0, 1));
		World = T * Ry;

		glm::vec3 c = glm::vec3(World * glm::vec4(0, camHeight + (camDist * sin(pitch)), camDist * cos(pitch), 1));
		glm::vec3 a = glm::vec3(World * glm::vec4(0, 0, 0, 1.0f)) + glm::vec3(0, camHeight, 0);
		glm::mat4 Mv = glm::lookAt(c, a, glm::vec3(0, 1, 0));
		glm::mat4 Mp = glm::perspective(FOVy, Ar, nearPlane, farPlane);
		Mp[1][1] *= -1;
		ViewPrj = Mp * Mv;

		// DAMPING
		float lambda = 10;
		if (ViewPrjOld == glm::mat4(1))
			ViewPrjOld = ViewPrj;
		ViewPrj = ViewPrjOld * exp(-lambda * deltaT) + ViewPrj * (1 - exp(-lambda * deltaT));
		ViewPrjOld = ViewPrj;
	}
};



//#include "Logic.hpp"

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