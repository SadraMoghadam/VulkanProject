#include "Starter.hpp"
#include "TextMaker.hpp"

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
	// Constant Parameters
	static const int mapSize = 20;
	const float collisionThreshold = 0.5f; // we can have multiple threshold for multiple objects
	static const int numOfPlants = 200;

	// Descriptor Set Layouts
	DescriptorSetLayout DSLGubo, DSLToon, DSLToonBlinn;

	// Vertex formats
	VertexDescriptor VMesh;

	// Pipelines
	Pipeline PToon, PToonBlinn;

	// Models
	Model<VertexMesh> MCharacter, MGround, MPlant;

	// Textures
	Texture TCharacter, TGround, TPlant;

	// Descriptor sets
	DescriptorSet DSGubo, DSCharacter, DSGround[4], DSPlant[numOfPlants];

	// Uniform Blocks
	UniformBufferObject uboCharacter, uboGround[4], uboPlant[numOfPlants];
	GlobalUniformBufferObject gubo;

	// Other Parameters
	int currentScene = 1;
	float Ar;
	float yaw = 0, pitch = .3f, roll = 0;
	glm::mat4 World, ViewPrj, GWorld, ViewPrjOld;
	glm::vec3 camPos = glm::vec3(0.0, 1.5, 0.0);
	float camAlpha = 0.0f, camBeta = 0.0f;
	// Environment Parameters
	std::list<glm::vec2> collisionPositions;
	float randX, randY, randRot;
	glm::vec2 groundPositions[4] = { {-1, -1}, {-1, 0}, {0, -1}, {0, 0} };
	glm::vec2 plantPositions[numOfPlants];
	float plantRotations[numOfPlants];



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
		uniformBlocksInPool = 2 + 4 * 2 + numOfPlants * 2;
		texturesInPool = 3;
		setsInPool = 2 + 4 * 2 + numOfPlants * 2;
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
		MPlant.init(this, &VMesh, "Models/plant.obj", OBJ);

		// Initializing Textures
		TCharacter.init(this, "textures/Wood.png");
		TGround.init(this, "textures/Ground.png");
		TPlant.init(this, "textures/Plant.png");
		// Init local variables
		CalculateCollisionPositions();
		CalculateEnvironmentObjectsPositionsAndRotations();
	}
	
	void pipelinesAndDescriptorSetsInit() 
	{
		/* TODO */
		// Creating Pipelines
		PToon.create();
		PToonBlinn.create();

		// Defining the Descriptor Sets
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBufferObject), nullptr}
			});
		DSCharacter.init(this, &DSLToon, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &TCharacter}
			});
		for (int i = 0; i < 4; i++)
		{
			DSGround[i].init(this, &DSLToonBlinn, {
						{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
						{1, TEXTURE, 0, &TGround}
				});
		}
		for (int i = 0; i < numOfPlants; i++) 
		{
			DSPlant[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TPlant},
				});
		}

	}

	void pipelinesAndDescriptorSetsCleanup() 
	{
		/* TODO */
		// Cleanup Pipelines
		PToon.cleanup();
		PToonBlinn.cleanup();

		// Cleanup Descriptor Sets
		DSGubo.cleanup();
		DSCharacter.cleanup();
		for (int i = 0; i < 4; i++)
		{
			DSGround[i].cleanup();
		}
		for (int i = 0; i < numOfPlants; i++)
			DSPlant[i].cleanup();
	}
	
	void localCleanup() 
	{
		/* TODO */
		// Cleanup Textures
		TCharacter.cleanup();
		TGround.cleanup();
		TPlant.cleanup();

		// Cleanup Models
		MCharacter.cleanup();
		MGround.cleanup();
		MPlant.cleanup();

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

		MPlant.bind(commandBuffer);
		for (int i = 0; i < numOfPlants; i++) {
			DSPlant[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MPlant.indices.size()), 1, 0, 0, 0);
		}


		// Ground
		PToonBlinn.bind(commandBuffer);
		MGround.bind(commandBuffer);
		for (int i = 0; i < 4; i++)
		{
			DSGround[i].bind(commandBuffer, PToonBlinn, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MGround.indices.size()), 1, 0, 0, 0);
		}
	}

	void updateUniformBuffer(uint32_t currentImage) 
	{
		/* TODO */
		static bool debounce = false;
		static int curDebounce = 0;

		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_SPACE;
				currentScene = (currentScene + 1) % 2;
				std::cout << "Scene : " << currentScene << "\n";
				RebuildPipeline();
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_SPACE) && debounce) {
				debounce = false;
				curDebounce = 0;
			}
		}

		switch (currentScene)
		{
		case 0:
			Spectate();
			break;
		case 1:
			PlayerMovement();
		}

		gubo.DlightDir = glm::vec3(cos(glm::radians(135.0f)), sin(glm::radians(135.0f)), 0.0f);
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.1f);
		gubo.eyePos = glm::vec3(100.0, 100.0, 100.0);

		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		RenderCharacter(currentImage);
		RenderEnvironment(currentImage);		
	}	

	void PlayerMovement()
	{
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 100.f;

		// Player starting point
		const glm::vec3 startingPosition = glm::vec3(0.0, 0.0, 0.0);

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
		getSixAxis(deltaT, m, r, fire);

		static glm::vec3 pos = startingPosition;
		glm::vec3 nextPos = pos;

		glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1));
		glm::vec3 uy = glm::vec3(0, 1, 0);
		glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1));
		pitch += ROT_SPEED * r.x * deltaT;
		yaw += ROT_SPEED * -r.y * deltaT;
		roll += ROT_SPEED * r.z * deltaT;

		glm::vec2 cp = { 5, 5 }; // CollisionPosition
		nextPos += ux * MOVE_SPEED * m.x * deltaT;
		nextPos += uz * MOVE_SPEED * m.z * deltaT;
		bool xCollision = nextPos.x < cp.x + collisionThreshold && nextPos.x > cp.x - collisionThreshold;
		bool yCollision = nextPos.z < cp.y + collisionThreshold && nextPos.z > cp.y - collisionThreshold;
		if (!(xCollision && yCollision))
		{
			pos += ux * MOVE_SPEED * m.x * deltaT;
			pos += uy * MOVE_SPEED * m.y * deltaT;
			pos += uz * MOVE_SPEED * m.z * deltaT;
		}
		//if (nextPos.x < cp.x + collisionThreshold && nextPos.x > cp.x - collisionThreshold)
		//{
		//	pos.x = cp.x - collisionThreshold;
		//}
		//if (nextPos.z < cp.y + collisionThreshold && nextPos.z > cp.y - collisionThreshold)
		//{
		//	pos.z = cp.y - collisionThreshold;
		//}
		//pos += ux * MOVE_SPEED * m.x * deltaT;
		//pos += uy * MOVE_SPEED * m.y * deltaT;
		//pos += uz * MOVE_SPEED * m.z * deltaT;
		//if (pos.x < cp.x + collisionThreshold && pos.x > cp.x - collisionThreshold) 
		//{
		//	pos.x = cp.x - collisionThreshold;
		//}
		//if (pos.z < cp.y + collisionThreshold && pos.z > cp.y - collisionThreshold)
		//{
		//	pos.z = cp.y - collisionThreshold;
		//}
		std::cout << pos.x << "-";
		glm::mat4 T = glm::translate(glm::mat4(1.0), pos);
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
		float lambda = 7;
		if (ViewPrjOld == glm::mat4(1))
			ViewPrjOld = ViewPrj;
		ViewPrj = ViewPrjOld * exp(-lambda * deltaT) + ViewPrj * (1 - exp(-lambda * deltaT));
		ViewPrjOld = ViewPrj;
	}



	void CalculateCollisionPositions()
	{
		collisionPositions.push_back({ -10, -10 });
	}

	void Spectate()
	{
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 10.0f;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);

		camAlpha = camAlpha - ROT_SPEED * deltaT * r.y;
		camBeta = camBeta - ROT_SPEED * deltaT * r.x;
		camBeta = camBeta < glm::radians(-90.0f) ? glm::radians(-90.0f) :
			(camBeta > glm::radians(90.0f) ? glm::radians(90.0f) : camBeta);

		glm::vec3 ux = glm::rotate(glm::mat4(1.0f), camAlpha, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1);
		glm::vec3 uz = glm::rotate(glm::mat4(1.0f), camAlpha, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1);
		camPos = camPos + MOVE_SPEED * m.x * ux * deltaT;
		camPos = camPos + MOVE_SPEED * m.y * glm::vec3(0, 1, 0) * deltaT;
		camPos = camPos + MOVE_SPEED * m.z * uz * deltaT;

		glm::mat4 M = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 50.0f);
		M[1][1] *= -1;

		glm::mat4 Mv = glm::rotate(glm::mat4(1.0), -camBeta, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0), -camAlpha, glm::vec3(0, 1, 0)) *
			glm::translate(glm::mat4(1.0), -camPos);

		ViewPrj = M * Mv;
	}

	void RenderCharacter(uint32_t currentImage)
	{
		glm::mat4 coefficient = currentScene == 0 ? glm::mat4(1) : World;
		GWorld = coefficient * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0, 1, 0));
		uboCharacter.amb = 1.0f; uboCharacter.gamma = 180.0f; uboCharacter.sColor = glm::vec3(1.0f);
		uboCharacter.mvpMat = ViewPrj * GWorld;
		uboCharacter.mMat = GWorld;
		uboCharacter.nMat = glm::inverse(glm::transpose(GWorld));
		DSCharacter.map(currentImage, &uboCharacter, sizeof(uboCharacter), 0);
	}

	void RenderEnvironment(uint32_t currentImage)
	{
		RenderGround(currentImage);
		RenderPlants(currentImage);
		RenderTrees(currentImage);
		RenderRocks(currentImage);
		RenderItems(currentImage);
	}

	void RenderGround(uint32_t currentImage)
	{
		for (int i = 0; i < 4; i++)
		{
			GWorld = glm::translate(glm::scale(glm::mat4(1), glm::vec3(mapSize / 2)), glm::vec3(groundPositions[i].x, 0, groundPositions[i].y));
			uboGround[i].amb = 1.0f; uboGround[i].gamma = 180.0f; uboGround[i].sColor = glm::vec3(1.0f);
			uboGround[i].mvpMat = ViewPrj * GWorld;
			uboGround[i].mMat = GWorld;
			uboGround[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSGround[i].map(currentImage, &uboGround[i], sizeof(uboGround[i]), 0);
		}
	}

	void RenderPlants(uint32_t currentImage) 
	{
		for (int i = 0; i < numOfPlants; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(plantPositions[i].x, 0, plantPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(plantRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
			uboPlant[i].amb = .7f; uboPlant[i].gamma = 180.0f; uboPlant[i].sColor = glm::vec3(1.0f);
			uboPlant[i].mvpMat = ViewPrj * GWorld;
			uboPlant[i].mMat = GWorld;
			uboPlant[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSPlant[i].map(currentImage, &uboPlant[i], sizeof(uboPlant[i]), 0);
		}
	}

	void RenderTrees(uint32_t currentImage) {}

	void RenderRocks(uint32_t currentImage) {}

	void RenderItems(uint32_t currentImage) {}

	void CalculateEnvironmentObjectsPositionsAndRotations()
	{
		// Plants
		for (int i = 0; i < numOfPlants; i++)
		{
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			plantPositions[i] = { randX, randY };
			plantRotations[i] = randRot;
		}
	}
};


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