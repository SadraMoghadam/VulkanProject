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
	static const int mapSize = 50;
	static const int numOfPlants = 200;
	static const int numOfFlowers = 200;
	static const int numOfMountains = 7 * 4;
	static const int numOfSmallRocks = 200;
	static const int numOfStumps = 15;
	static const int numOfClouds = 15;
	static const int numOfRocks = 15;
	static const int numOfTrees = 10;
	static const int numOfTrees1 = 10;
	static const int numOfTrees2 = 10;
	static const int numOfTrees3 = 10;
	static const int numOfTrees4 = 10;
	static const int numOfItems = 5;
	static const int numOfCollisions = numOfRocks + numOfStumps + numOfTrees * 5;

	// Descriptor Set Layouts
	DescriptorSetLayout DSLGubo, DSLToon, DSLToonBlinn;

	// Vertex formats
	VertexDescriptor VMesh;

	// Pipelines
	Pipeline PToon, PToonBlinn;

	// Models
	Model<VertexMesh> MCharacter, MGround, MPlant, MFlower, MMountain, MSmallRock, 
		MStump, MCloud, MRock, MTree, MTree1, MTree2, MTree3, MTree4, MItem;

	// Textures
	Texture TCharacter, TGround, TEnv, TEnv2, TItem;

	// Descriptor sets
	DescriptorSet DSGubo, DSCharacter, DSGround[4], DSPlant[numOfPlants], DSFlower[numOfFlowers], 
		DSMountain[numOfMountains], DSSmallRock[numOfSmallRocks], DSStump[numOfStumps], 
		DSCloud[numOfClouds], DSRock[numOfRocks], DSTree[numOfTrees], DSTree1[numOfTrees1], 
		DSTree2[numOfTrees2], DSTree3[numOfTrees3], DSTree4[numOfTrees4], DSItem[numOfItems];

	// Uniform Blocks
	UniformBufferObject uboCharacter, uboGround[4], uboPlant[numOfPlants], uboFlower[numOfFlowers], 
		uboMountain[numOfMountains], uboSmallRock[numOfSmallRocks], uboStump[numOfStumps], 
		uboCloud[numOfClouds], uboRock[numOfRocks], uboTree[numOfTrees], uboTree1[numOfTrees1], 
		uboTree2[numOfTrees2], uboTree3[numOfTrees3], uboTree4[numOfTrees4], uboItem[numOfItems];
	GlobalUniformBufferObject gubo;

	// Other Parameters
	int currentScene = 1;
	float Ar;
	float yaw = 0, pitch = .3f, roll = 0;
	glm::mat4 World, ViewPrj, GWorld, ViewPrjOld;
	glm::vec3 camPos = glm::vec3(0.0, 1.5, 0.0);
	float camAlpha = 0.0f, camBeta = 0.0f;
	glm::vec3 characterRotation = { 0.0f, 90.0f , 0.0f};
	glm::vec3 realNormX = { 1, 0, 0 };
	glm::vec3 realNormY = { 0, 1, 0 };
	glm::vec3 realNormZ = { 0, 0, 1 };

	// Environment Parameters
	float randX, randY, randRot, randZ;
	glm::vec2 groundPositions[4] = { {-1, -1}, {-1, 0}, {0, -1}, {0, 0} };
	glm::vec2 plantPositions[numOfPlants];
	float plantRotations[numOfPlants];
	glm::vec2 flowerPositions[numOfFlowers];
	float flowerRotations[numOfFlowers];
	glm::vec2 mountainPositions[numOfMountains];
	float mountainRotations[numOfMountains];
	float mountainScales[numOfMountains];
	glm::vec2 smallRockPositions[numOfSmallRocks];
	float smallRockRotations[numOfSmallRocks];
	glm::vec2 stumpPositions[numOfStumps];
	float stumpRotations[numOfStumps];
	float stumpScales[numOfStumps];
	glm::vec3 cloudPositions[numOfClouds];
	float cloudRotations[numOfClouds];
	float cloudScales[numOfClouds];
	glm::vec2 rockPositions[numOfRocks];
	float rockRotations[numOfRocks];
	float rockScales[numOfRocks];
	glm::vec2 treePositions[numOfTrees];
	float treeRotations[numOfTrees];
	float treeScales[numOfTrees];
	glm::vec2 tree1Positions[numOfTrees1];
	float tree1Rotations[numOfTrees1];
	float tree1Scales[numOfTrees1];
	glm::vec2 tree2Positions[numOfTrees2];
	float tree2Rotations[numOfTrees2];
	float tree2Scales[numOfTrees2];
	glm::vec2 tree3Positions[numOfTrees3];
	float tree3Rotations[numOfTrees3];
	float tree3Scales[numOfTrees3];
	glm::vec2 tree4Positions[numOfTrees4];
	float tree4Rotations[numOfTrees4];
	float tree4Scales[numOfTrees4];
	glm::vec2 itemPositions[numOfItems];
	float itemRotations[numOfItems];

	//Jump params
	bool isJumping = FALSE;
	float VJumpIni = .5f;
	float VJump = VJumpIni;
	float g = -1.1f;

	// Collision Parameters
	bool xCollision = false, yCollision = false, isCollision = false;
	int thresholdIndex = 0;
	float mountainThreshold = 7.8f;
	float mountainThresholdCoefficient = 10;
	std::tuple<glm::vec2, float> collisionsInfo[numOfCollisions];
	float rockThreshold = 0.7f;
	float rockThresholdCoefficient = 1;
	float stumpThreshold = 0.4f;
	float stumpThresholdCoefficient = 1;
	float treeThreshold = 0.3f;
	float treeThresholdCoefficient = 1;


	void setWindowParameters()
	{
		windowWidth = 800;
		windowHeight = 600;
		windowTitle = "Jungle Exploration";
		windowResizable = GLFW_TRUE;
		initialBackgroundColor = { 0.5f, 0.8f, 0.9f, 1.0f };

		Ar = (float)windowWidth / (float)windowHeight;
	}

	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}

	void setDescriptorPool()
	{
		uniformBlocksInPool = 2 + 4 * 2 + numOfPlants * 2 + numOfFlowers * 2 + numOfMountains * 2 + 
			numOfSmallRocks * 2 + numOfStumps * 2 + numOfClouds * 2 + numOfRocks * 2 + numOfTrees * 2 * 5 + numOfItems * 2;
		texturesInPool = 5;
		setsInPool = 2 + 4 * 2 + numOfPlants * 2 + numOfFlowers * 2 + numOfMountains * 2 +
			numOfSmallRocks * 2 + numOfStumps * 2 + numOfClouds * 2 + numOfRocks * 2 + numOfTrees * 2 * 5 + numOfItems * 2;
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
		MFlower.init(this, &VMesh, "Models/flower.obj", OBJ);
		MMountain.init(this, &VMesh, "Models/mountain.obj", OBJ);
		MSmallRock.init(this, &VMesh, "Models/smallrock.obj", OBJ);
		MStump.init(this, &VMesh, "Models/stump.obj", OBJ);
		MCloud.init(this, &VMesh, "Models/cloud.obj", OBJ);
		MRock.init(this, &VMesh, "Models/rock2.obj", OBJ);
		MTree.init(this, &VMesh, "Models/tree.obj", OBJ);
		MTree1.init(this, &VMesh, "Models/tree1.obj", OBJ);
		MTree2.init(this, &VMesh, "Models/tree2.obj", OBJ);
		MTree3.init(this, &VMesh, "Models/tree3.obj", OBJ);
		MTree4.init(this, &VMesh, "Models/tree4.obj", OBJ);
		MItem.init(this, &VMesh, "Models/red.obj", OBJ);

		// Initializing Textures
		TCharacter.init(this, "textures/Wood.png");
		TGround.init(this, "textures/Ground.png");
		TEnv.init(this, "textures/Texture_01.jpg");
		TEnv2.init(this, "textures/Terrain-Texture_2.png");
		TItem.init(this, "textures/Wood.png");
		// Init local variables
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
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfFlowers; i++)
		{
			DSFlower[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfMountains; i++)
		{
			DSMountain[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfSmallRocks; i++)
		{
			DSSmallRock[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfStumps; i++)
		{
			DSStump[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfClouds; i++)
		{
			DSCloud[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfRocks; i++)
		{
			DSRock[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees; i++)
		{
			DSTree[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfTrees1; i++)
		{
			DSTree1[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees2; i++)
		{
			DSTree2[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees3; i++)
		{
			DSTree3[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees4; i++)
		{
			DSTree4[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfItems; i++)
		{
			DSItem[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(UniformBufferObject), nullptr},
					{1, TEXTURE, 0, &TItem},
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
		for (int i = 0; i < numOfFlowers; i++)
			DSFlower[i].cleanup();
		for (int i = 0; i < numOfMountains; i++)
			DSMountain[i].cleanup();
		for (int i = 0; i < numOfSmallRocks; i++)
			DSSmallRock[i].cleanup();
		for (int i = 0; i < numOfStumps; i++)
			DSStump[i].cleanup();
		for (int i = 0; i < numOfClouds; i++)
			DSCloud[i].cleanup();
		for (int i = 0; i < numOfRocks; i++)
			DSRock[i].cleanup();
		for (int i = 0; i < numOfTrees; i++)
			DSTree[i].cleanup();
		for (int i = 0; i < numOfTrees1; i++)
			DSTree1[i].cleanup();
		for (int i = 0; i < numOfTrees2; i++)
			DSTree2[i].cleanup();
		for (int i = 0; i < numOfTrees3; i++)
			DSTree3[i].cleanup();
		for (int i = 0; i < numOfTrees4; i++)
			DSTree4[i].cleanup();
		for (int i = 0; i < numOfItems; i++)
			DSItem[i].cleanup();

	}

	void localCleanup()
	{
		/* TODO */
		// Cleanup Textures
		TCharacter.cleanup();
		TGround.cleanup();
		TEnv.cleanup();
		TEnv2.cleanup();
		TItem.cleanup();

		// Cleanup Models
		MCharacter.cleanup();
		MGround.cleanup();
		MPlant.cleanup();
		MFlower.cleanup();
		MMountain.cleanup();
		MSmallRock.cleanup();
		MStump.cleanup();
		MCloud.cleanup();
		MRock.cleanup();
		MTree.cleanup();
		MTree1.cleanup();
		MTree2.cleanup();
		MTree3.cleanup();
		MTree4.cleanup();
		MItem.cleanup();

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

		MFlower.bind(commandBuffer);
		for (int i = 0; i < numOfFlowers; i++) {
			DSFlower[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MFlower.indices.size()), 1, 0, 0, 0);
		}

		MMountain.bind(commandBuffer);
		for (int i = 0; i < numOfMountains; i++) {
			DSMountain[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MMountain.indices.size()), 1, 0, 0, 0);
		}

		MSmallRock.bind(commandBuffer);
		for (int i = 0; i < numOfSmallRocks; i++) {
			DSSmallRock[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSmallRock.indices.size()), 1, 0, 0, 0);
		}

		MStump.bind(commandBuffer);
		for (int i = 0; i < numOfStumps; i++) {
			DSStump[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MStump.indices.size()), 1, 0, 0, 0);
		}

		MCloud.bind(commandBuffer);
		for (int i = 0; i < numOfClouds; i++) {
			DSCloud[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MCloud.indices.size()), 1, 0, 0, 0);
		}

		MRock.bind(commandBuffer);
		for (int i = 0; i < numOfRocks; i++) {
			DSRock[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MRock.indices.size()), 1, 0, 0, 0);
		}

		MTree.bind(commandBuffer);
		for (int i = 0; i < numOfTrees; i++) {
			DSTree[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTree.indices.size()), 1, 0, 0, 0);
		}

		MTree1.bind(commandBuffer);
		for (int i = 0; i < numOfTrees1; i++) {
			DSTree1[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTree1.indices.size()), 1, 0, 0, 0);
		}

		MTree2.bind(commandBuffer);
		for (int i = 0; i < numOfTrees2; i++) {
			DSTree2[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTree2.indices.size()), 1, 0, 0, 0);
		}

		MTree3.bind(commandBuffer);
		for (int i = 0; i < numOfTrees3; i++) {
			DSTree3[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTree3.indices.size()), 1, 0, 0, 0);
		}

		MTree4.bind(commandBuffer);
		for (int i = 0; i < numOfTrees4; i++) {
			DSTree4[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MTree4.indices.size()), 1, 0, 0, 0);
		}

		MItem.bind(commandBuffer);
		for (int i = 0; i < numOfItems; i++) {
			DSItem[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MItem.indices.size()), 1, 0, 0, 0);
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

		if (glfwGetKey(window, GLFW_KEY_M)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_M;
				currentScene = (currentScene + 1) % 2;
				//std::cout << "Scene : " << currentScene << "\n";
				RebuildPipeline();
			}
		}
		else {
			if ((curDebounce == GLFW_KEY_M) && debounce) {
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

	void PlayerJump(float deltaT, glm::vec3& m)
	{
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			isJumping = TRUE;
			//m.y = 1.0f;
		}
	}

	void PlayerMovement()
	{
		const float FOVy = glm::radians(45.0f);
		const float nearPlane = 0.1f;
		const float farPlane = 1000.f;

		// Player starting point
		const glm::vec3 startingPosition = glm::vec3(0.0, 0.0, 0.0);

		// Camera target height and distance
		const float camHeight = 0.25;
		const float camDist = 1.5;
		// Camera Pitch limits
		const float minPitch = glm::radians(-5.0f);
		const float maxPitch = glm::radians(60.0f);
		// Rotation and motion speed
		const float ROT_SPEED = glm::radians(120.0f);
		const float MOVE_SPEED = 5.0f;

		float deltaT;
		glm::vec3 m = glm::vec3(0.0f, static_cast<int>(isJumping), 0.0f), r = glm::vec3(0.0f);
		bool fire = false;
		getSixAxis(deltaT, m, r, fire);
		PlayerJump(deltaT, m);

		static glm::vec3 pos = startingPosition;
		glm::vec3 nextPos = pos;

		if (isJumping) {
			VJump += g * deltaT;
			//pos.y += VJump* deltaT * MOVE_SPEED * m.y;
		}

		glm::vec3 ux = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(1, 0, 0, 1));
		glm::vec3 uy = glm::vec3(0, VJump, 0);
		glm::vec3 uz = glm::vec3(glm::rotate(glm::mat4(1), yaw, glm::vec3(0, 1, 0)) * glm::vec4(0, 0, -1, 1));
		pitch += ROT_SPEED * r.x * deltaT;
		yaw += ROT_SPEED * -r.y * deltaT;
		roll += ROT_SPEED * r.z * deltaT;
		//std::cout << pitch << "-";
		glm::vec2 cp = { 5, 5 }; // CollisionPosition
		nextPos += ux * MOVE_SPEED * m.x * deltaT;
		nextPos += uz * MOVE_SPEED * m.z * deltaT;

		MapBorderCollisionHandler(pos, nextPos);
		CollisionChecker(nextPos);
		//std::cout << uy.y;
		if (!isCollision)
		{
			pos += ux * MOVE_SPEED * m.x * deltaT;
			pos += uz * MOVE_SPEED * m.z * deltaT;
			//characterRotation += realNormX * MOVE_SPEED * m.x * deltaT;
			//characterRotation += realNormZ * MOVE_SPEED * m.z * deltaT;
			std::cout << pos.x << "=" << pos.z << " & ";
		}
		pos += uy * MOVE_SPEED * m.y * deltaT;

		if (pos.y < 0.0f) {
			isJumping = FALSE;
			VJump = VJumpIni;
		}
		isCollision = false;

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
		float lambda = 20;
		if (ViewPrjOld == glm::mat4(1))
			ViewPrjOld = ViewPrj;
		ViewPrj = ViewPrjOld * exp(-lambda * deltaT) + ViewPrj * (1 - exp(-lambda * deltaT));
		ViewPrjOld = ViewPrj;
	}


	void MapBorderCollisionHandler(glm::vec3& pos, glm::vec3& nextPos)
	{
		if (nextPos.x < -mapSize / 2 + mountainThreshold)
		{
			pos.x = -mapSize / 2 + mountainThreshold;
		}
		else if (nextPos.x > mapSize / 2 - mountainThreshold)
		{
			pos.x = mapSize / 2 - mountainThreshold;
		}
		if (nextPos.z < -mapSize / 2 + mountainThreshold)
		{
			pos.z = -mapSize / 2 + mountainThreshold;
		}
		else if (nextPos.z > mapSize / 2 - mountainThreshold)
		{
			pos.z = mapSize / 2 - mountainThreshold;
		}
		if (nextPos.y < 0)
		{
			pos.y = 0;
		}
	}

	void CollisionChecker(glm::vec3 nextPos)
	{
		for (int i = 0; i < numOfCollisions; i++)
		{
			//std::cout << std::get<1>(collisionsInfo[i]) << "-" << std::get<0>(collisionsInfo[i]).x << " && ";
			xCollision = nextPos.x < std::get<0>(collisionsInfo[i]).x + std::get<1>(collisionsInfo[i]) && nextPos.x > std::get<0>(collisionsInfo[i]).x - std::get<1>(collisionsInfo[i]);
			yCollision = nextPos.z < std::get<0>(collisionsInfo[i]).y + std::get<1>(collisionsInfo[i]) && nextPos.z > std::get<0>(collisionsInfo[i]).y - std::get<1>(collisionsInfo[i]);
			if (xCollision && yCollision)
			{
				isCollision = true;
				break;
			}
		}
	}

	void AddCollisionPoint(glm::vec2 position, float threshold)
	{
		std::tuple<glm::vec2, float> collisionInfo(position, threshold);
		collisionsInfo[thresholdIndex++] = collisionInfo;
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
		GWorld = coefficient * glm::rotate(glm::mat4(1.0f), glm::radians(characterRotation.y), realNormY);
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
		RenderFlowers(currentImage);
		RenderMountains(currentImage);
		RenderSmallRocks(currentImage);
		RenderStumps(currentImage);
		RenderClouds(currentImage);
		RenderTrees(currentImage);
		RenderTrees1(currentImage);
		RenderTrees2(currentImage);
		RenderTrees3(currentImage);
		RenderTrees4(currentImage);
		RenderRocks(currentImage);
		RenderItems(currentImage);
	}

	void RenderGround(uint32_t currentImage)
	{
		for (int i = 0; i < 4; i++)
		{
			GWorld = glm::translate(glm::scale(glm::mat4(1), glm::vec3(mapSize / 2)), glm::vec3(groundPositions[i].x, 0, groundPositions[i].y));
			SetUboDs(currentImage, uboGround, DSGround, i);
		}
	}

	void RenderPlants(uint32_t currentImage)
	{
		for (int i = 0; i < numOfPlants; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(plantPositions[i].x, 0, plantPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(plantRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
			SetUboDs(currentImage, uboPlant, DSPlant, i, 0.7f);
		}
	}

	void RenderFlowers(uint32_t currentImage)
	{
		for (int i = 0; i < numOfFlowers; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(flowerPositions[i].x, 0, flowerPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(flowerRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
			SetUboDs(currentImage, uboFlower, DSFlower, i);
		}
	}

	void RenderMountains(uint32_t currentImage)
	{
		for (int i = 0; i < numOfMountains; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(mountainPositions[i].x, 0, mountainPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(mountainRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(mountainScales[i] * mapSize / 20));
			SetUboDs(currentImage, uboMountain, DSMountain, i);
		}
	}

	void RenderSmallRocks(uint32_t currentImage)
	{
		for (int i = 0; i < numOfSmallRocks; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(smallRockPositions[i].x, 0, smallRockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(smallRockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.1f));
			SetUboDs(currentImage, uboSmallRock, DSSmallRock, i);
		}
	}

	void RenderStumps(uint32_t currentImage)
	{
		for (int i = 0; i < numOfStumps; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(stumpPositions[i].x, 0, stumpPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(stumpRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(stumpScales[i] * mapSize / 20));
			SetUboDs(currentImage, uboStump, DSStump, i);
		}
	}

	void RenderClouds(uint32_t currentImage)
	{
		for (int i = 0; i < numOfClouds; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(cloudPositions[i].x, cloudPositions[i].z, cloudPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(cloudRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(cloudScales[i] * mapSize / 20));
			SetUboDs(currentImage, uboCloud, DSCloud, i);
		}
	}

	void RenderRocks(uint32_t currentImage)
	{
		for (int i = 0; i < numOfRocks; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(rockPositions[i].x, 0, rockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(rockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(rockScales[i] * mapSize / 20));
			SetUboDs(currentImage, uboRock, DSRock, i);
		}
	}

	void RenderTrees(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(treePositions[i].x, 0, treePositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(treeRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(treeScales[i] * mapSize / 20));
			SetUboDs(currentImage, uboTree, DSTree, i);
		}
	}

	void RenderTrees1(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees1; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree1Positions[i].x, 0, tree1Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree1Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree1Scales[i] * mapSize / 20));
			SetUboDs(currentImage, uboTree1, DSTree1, i);
		}
	}

	void RenderTrees2(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees2; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree2Positions[i].x, 0, tree2Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree2Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree2Scales[i] * mapSize / 20));
			SetUboDs(currentImage, uboTree2, DSTree2, i);
		}
	}

	void RenderTrees3(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees3; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree3Positions[i].x, 0, tree3Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree3Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree3Scales[i] * mapSize / 20));
			SetUboDs(currentImage, uboTree3, DSTree3, i);
		}
	}

	void RenderTrees4(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees4; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree4Positions[i].x, 0, tree4Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree4Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree4Scales[i] * mapSize / 20));
			SetUboDs(currentImage, uboTree4, DSTree4, i);
		}
	}

	void RenderItems(uint32_t currentImage) {
		for (int i = 0; i < numOfItems; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(itemPositions[i].x, 0, itemPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(itemRotations[i]), glm::vec3(0, 1, 0));
			SetUboDs(currentImage, uboItem, DSItem, i);
		}
	}

	void SetUboDs(uint32_t currentImage, UniformBufferObject ubo[], DescriptorSet DS[], int index, float amb = 1.0f, float gamma = 180.0f, glm::vec3 sColor = glm::vec3(1.0f))
	{
		ubo[index].amb = amb; ubo[index].gamma = gamma; ubo[index].sColor = sColor;
		ubo[index].mvpMat = ViewPrj * GWorld;
		ubo[index].mMat = GWorld;
		ubo[index].nMat = glm::inverse(glm::transpose(GWorld));
		DS[index].map(currentImage, &ubo[index], sizeof(ubo[index]), 0);
	}

	void CalculateEnvironmentObjectsPositionsAndRotations()
	{
		srand(time(0));
		// Plants
		for (int i = 0; i < numOfPlants; i++)
		{
			CalculateRandomPositionsRotations();
			plantPositions[i] = { randX, randY };
			plantRotations[i] = randRot;
		}

		// Flowers
		for (int i = 0; i < numOfFlowers; i++)
		{
			CalculateRandomPositionsRotations();
			flowerPositions[i] = { randX, randY };
			flowerRotations[i] = randRot;
		}

		// Mountains
		for (int i = 0; i < numOfMountains; i++)
		{
			mountainScales[i] = (float)rand() / RAND_MAX / 6 + 0.15;
			mountainRotations[i] = rand() % (360 + 1);
		}
		for (int i = 0; i < numOfMountains / 4; i++)
		{
			int mountainsPerSide = numOfMountains / 4;
			mountainPositions[i] = { -mapSize / 2 + i * mapSize / mountainsPerSide + 2, -mapSize / 2 };
			mountainPositions[i + 1 * numOfMountains / 4] = { -mapSize / 2, -mapSize / 2 + i * mapSize / mountainsPerSide + 2 };
			mountainPositions[i + 2 * numOfMountains / 4] = { -mapSize / 2 + i * mapSize / mountainsPerSide + 2, mapSize / 2 };
			mountainPositions[i + 3 * numOfMountains / 4] = { mapSize / 2, -mapSize / 2 + i * mapSize / mountainsPerSide + 2 };

			//AddCollisionPoint(mountainPositions[i], mountainThreshold + (mountainScales[i] - .15f) * mountainThresholdCoefficient);
			//AddCollisionPoint(mountainPositions[i + 1 * numOfMountains / 4], mountainThreshold + (mountainScales[i + 1 * numOfMountains / 4] - .15f) * mountainThresholdCoefficient);
			//AddCollisionPoint(mountainPositions[i + 2 * numOfMountains / 4], mountainThreshold + (mountainScales[i + 1 * numOfMountains / 4] - .15f) * mountainThresholdCoefficient);
			//AddCollisionPoint(mountainPositions[i + 3 * numOfMountains / 4], mountainThreshold + (mountainScales[i + 1 * numOfMountains / 4] - .15f) * mountainThresholdCoefficient);
		}

		// SmallRocks
		for (int i = 0; i < numOfSmallRocks; i++)
		{
			CalculateRandomPositionsRotations();
			smallRockPositions[i] = { randX, randY };
			smallRockRotations[i] = randRot;
		}

		// Stumps
		for (int i = 0; i < numOfStumps; i++)
		{
			stumpScales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			stumpPositions[i] = { randX, randY };
			stumpRotations[i] = randRot;
			AddCollisionPoint(stumpPositions[i], stumpThreshold + (stumpScales[i] - 0.2f) * stumpThresholdCoefficient);
		}

		// Clouds
		for (int i = 0; i < numOfClouds; i++)
		{
			cloudScales[i] = (float)rand() / RAND_MAX / 6 + 0.15;
			CalculateRandomPositionsRotations(-mapSize / 2 - mountainThreshold , mapSize / 2 + mountainThreshold);
			randZ = (rand() < RAND_MAX / 2) ? mapSize / 3 : mapSize / 3 + 6;
			cloudPositions[i] = { randX, randY, randZ };
			cloudRotations[i] = randRot;
		}

		// Rocks
		for (int i = 0; i < numOfRocks; i++)
		{
			rockScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			rockPositions[i] = { randX, randY };
			rockRotations[i] = randRot;
			AddCollisionPoint(rockPositions[i], rockThreshold + (rockScales[i] - 0.05f) * rockThresholdCoefficient);
		}

		// Trees
		for (int i = 0; i < numOfTrees; i++)
		{
			treeScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			treePositions[i] = { randX, randY };
			treeRotations[i] = randRot;
			AddCollisionPoint(treePositions[i], treeThreshold + (treeScales[i] - 0.05f) * treeThresholdCoefficient);
		}

		// Trees1
		for (int i = 0; i < numOfTrees1; i++)
		{
			tree1Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			tree1Positions[i] = { randX, randY };
			tree1Rotations[i] = randRot;
			AddCollisionPoint(tree1Positions[i], treeThreshold + (tree1Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees2
		for (int i = 0; i < numOfTrees2; i++)
		{
			tree2Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			tree2Positions[i] = { randX, randY };
			tree2Rotations[i] = randRot;
			AddCollisionPoint(tree2Positions[i], treeThreshold + (tree2Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees3
		for (int i = 0; i < numOfTrees3; i++)
		{
			tree3Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			tree3Positions[i] = { randX, randY };
			tree3Rotations[i] = randRot;
			AddCollisionPoint(tree3Positions[i], treeThreshold + (tree3Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees4
		for (int i = 0; i < numOfTrees4; i++)
		{
			tree4Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold);
			tree4Positions[i] = { randX, randY };
			tree4Rotations[i] = randRot;
			AddCollisionPoint(tree4Positions[i], treeThreshold + (tree4Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Items
		for (int i = 0; i < numOfItems; i++)
		{
			// check item position not to go under another object
			CalculateItemsPosition();
			std::cout << randX << "=" << randY << " & ";
			itemPositions[i] = { randX, randY };
			itemRotations[i] = randRot;
		}
	}

	void CalculateItemsPosition()
	{
		// check item position not to go under another object
		bool overlap = false;
		while (true)
		{
			overlap = false;
			CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold + 1, mapSize / 2 - mountainThreshold - 1);
			CheckItemPositionOverlap(numOfStumps, stumpThreshold, stumpThresholdCoefficient, stumpPositions, stumpScales, overlap);
			CheckItemPositionOverlap(numOfRocks, rockThreshold, rockThresholdCoefficient, rockPositions, rockScales, overlap);
			CheckItemPositionOverlap(numOfTrees, treeThreshold, treeThresholdCoefficient, treePositions, treeScales, overlap);
			CheckItemPositionOverlap(numOfTrees1, treeThreshold, treeThresholdCoefficient, tree1Positions, tree1Scales, overlap);
			CheckItemPositionOverlap(numOfTrees2, treeThreshold, treeThresholdCoefficient, tree2Positions, tree2Scales, overlap);
			CheckItemPositionOverlap(numOfTrees3, treeThreshold, treeThresholdCoefficient, tree3Positions, tree3Scales, overlap);
			CheckItemPositionOverlap(numOfTrees4, treeThreshold, treeThresholdCoefficient, tree4Positions, tree4Scales, overlap);
			if (!overlap)
				break;
		}
	}

	void CheckItemPositionOverlap(int numOfObjects, float objectThreshold, float objectThresholdCoefficient, glm::vec2 objectPositions[], float objectScales[], bool& overlap)
	{
		float threshold = 0;
		for (int i = 0; i < numOfObjects; i++)
		{
			threshold = objectThreshold + (objectScales[i] - 0.2f) * objectThresholdCoefficient;
			if (overlap || ((randX < objectPositions[i].x + threshold && randX > objectPositions[i].x - threshold) &&
				(randY < objectPositions[i].y + threshold && randY > objectPositions[i].y - threshold)))
			{
				overlap = true;
				break;
			}
		}
	}

	void CalculateRandomPositionsRotations(int start = -mapSize / 2 + 1, int end = mapSize / 2 - 1)
	{
		randX = rand() % (end - start + 1) + start;
		randY = rand() % (end - start + 1) + start;
		randRot = rand() % (360 + 1);
	}
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