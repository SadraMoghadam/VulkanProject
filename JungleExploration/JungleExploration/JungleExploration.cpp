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
	static const int numOfCollisions = numOfRocks + numOfStumps + numOfTrees * 5;

	// Descriptor Set Layouts
	DescriptorSetLayout DSLGubo, DSLToon, DSLToonBlinn;

	// Vertex formats
	VertexDescriptor VMesh;

	// Pipelines
	Pipeline PToon, PToonBlinn;

	// Models
	Model<VertexMesh> MCharacter, MGround, MPlant, MFlower, MMountain, MSmallRock, MStump, MCloud, MRock, MTree, MTree1, MTree2, MTree3, MTree4;

	// Textures
	Texture TCharacter, TGround, TEnv, TEnv2;

	// Descriptor sets
	DescriptorSet DSGubo, DSCharacter, DSGround[4], DSPlant[numOfPlants], DSFlower[numOfFlowers], DSMountain[numOfMountains], DSSmallRock[numOfSmallRocks], DSStump[numOfStumps], DSCloud[numOfClouds], DSRock[numOfRocks], DSTree[numOfTrees], DSTree1[numOfTrees1], DSTree2[numOfTrees2], DSTree3[numOfTrees3], DSTree4[numOfTrees4];

	// Uniform Blocks
	UniformBufferObject uboCharacter, uboGround[4], uboPlant[numOfPlants], uboFlower[numOfFlowers], uboMountain[numOfMountains], uboSmallRock[numOfSmallRocks], uboStump[numOfStumps], uboCloud[numOfClouds], uboRock[numOfRocks], uboTree[numOfTrees], uboTree1[numOfTrees1], uboTree2[numOfTrees2], uboTree3[numOfTrees3], uboTree4[numOfTrees4];
	GlobalUniformBufferObject gubo;

	// Other Parameters
	int currentScene = 1;
	float Ar;
	float yaw = 0, pitch = .3f, roll = 0;
	glm::mat4 World, ViewPrj, GWorld, ViewPrjOld;
	glm::vec3 camPos = glm::vec3(0.0, 1.5, 0.0);
	float camAlpha = 0.0f, camBeta = 0.0f;
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
		initialBackgroundColor = { 0.0f, 0.0f, 0.01f, 1.0f };

		Ar = (float)windowWidth / (float)windowHeight;
	}

	void onWindowResize(int w, int h) {
		Ar = (float)w / (float)h;
	}

	void setDescriptorPool()
	{
		uniformBlocksInPool = 2 + 4 * 2 + numOfPlants * 2 + numOfFlowers * 2 + numOfMountains * 2 + numOfSmallRocks * 2 + numOfStumps * 2 + numOfClouds * 2 + numOfRocks * 2 + numOfTrees * 2 * 5;
		texturesInPool = 4;
		setsInPool = 2 + 4 * 2 + numOfPlants * 2 + numOfFlowers * 2 + numOfMountains * 2 + numOfSmallRocks * 2 + numOfStumps * 2 + numOfClouds * 2 + numOfRocks * 2 + numOfTrees * 2 * 5;
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

		// Initializing Textures
		TCharacter.init(this, "textures/Wood.png");
		TGround.init(this, "textures/Ground.png");
		TEnv.init(this, "textures/Texture_01.jpg");
		TEnv2.init(this, "textures/Terrain-Texture_2.png");
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
		for (int i = 0; i < numOfSmallRocks; i++)
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

	}

	void localCleanup()
	{
		/* TODO */
		// Cleanup Textures
		TCharacter.cleanup();
		TGround.cleanup();
		TEnv.cleanup();
		TEnv2.cleanup();

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
				//std::cout << "Scene : " << currentScene << "\n";
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

	void PlayerJump(float deltaT, glm::vec3& m)
	{
		if (glfwGetKey(window, GLFW_KEY_C)) {
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
		const float minPitch = glm::radians(-8.75f);
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
		//std::cout << nextPos.x << "-";

		MapBorderCollisionHandler(pos, nextPos);
		CollisionChecker(nextPos);
		std::cout << uy.y;
		if (!isCollision)
		{
			pos += ux * MOVE_SPEED * m.x * deltaT;
			pos += uz * MOVE_SPEED * m.z * deltaT;
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
		float lambda = 7;
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

	void RenderFlowers(uint32_t currentImage)
	{
		for (int i = 0; i < numOfFlowers; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(flowerPositions[i].x, 0, flowerPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(flowerRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
			uboFlower[i].amb = .7f; uboFlower[i].gamma = 180.0f; uboFlower[i].sColor = glm::vec3(1.0f);
			uboFlower[i].mvpMat = ViewPrj * GWorld;
			uboFlower[i].mMat = GWorld;
			uboFlower[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSFlower[i].map(currentImage, &uboFlower[i], sizeof(uboFlower[i]), 0);
		}
	}

	void RenderMountains(uint32_t currentImage)
	{
		for (int i = 0; i < numOfMountains; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(mountainPositions[i].x, 0, mountainPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(mountainRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(mountainScales[i] * mapSize / 20));
			uboMountain[i].amb = 1.0f; uboMountain[i].gamma = 180.0f; uboMountain[i].sColor = glm::vec3(1.0f);
			uboMountain[i].mvpMat = ViewPrj * GWorld;
			uboMountain[i].mMat = GWorld;
			uboMountain[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSMountain[i].map(currentImage, &uboMountain[i], sizeof(uboMountain[i]), 0);
		}
	}

	void RenderSmallRocks(uint32_t currentImage)
	{
		for (int i = 0; i < numOfSmallRocks; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(smallRockPositions[i].x, 0, smallRockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(smallRockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.1f));
			uboSmallRock[i].amb = .7f; uboSmallRock[i].gamma = 180.0f; uboSmallRock[i].sColor = glm::vec3(1.0f);
			uboSmallRock[i].mvpMat = ViewPrj * GWorld;
			uboSmallRock[i].mMat = GWorld;
			uboSmallRock[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSSmallRock[i].map(currentImage, &uboSmallRock[i], sizeof(uboSmallRock[i]), 0);
		}
	}

	void RenderStumps(uint32_t currentImage)
	{
		for (int i = 0; i < numOfStumps; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(stumpPositions[i].x, 0, stumpPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(stumpRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(stumpScales[i] * mapSize / 20));
			uboStump[i].amb = .7f; uboStump[i].gamma = 180.0f; uboStump[i].sColor = glm::vec3(1.0f);
			uboStump[i].mvpMat = ViewPrj * GWorld;
			uboStump[i].mMat = GWorld;
			uboStump[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSStump[i].map(currentImage, &uboStump[i], sizeof(uboStump[i]), 0);
		}
	}

	void RenderClouds(uint32_t currentImage)
	{
		for (int i = 0; i < numOfClouds; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(cloudPositions[i].x, cloudPositions[i].z, cloudPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(cloudRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(cloudScales[i] * mapSize / 20));
			uboCloud[i].amb = .7f; uboCloud[i].gamma = 180.0f; uboCloud[i].sColor = glm::vec3(1.0f);
			uboCloud[i].mvpMat = ViewPrj * GWorld;
			uboCloud[i].mMat = GWorld;
			uboCloud[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSCloud[i].map(currentImage, &uboCloud[i], sizeof(uboCloud[i]), 0);
		}
	}

	void RenderRocks(uint32_t currentImage)
	{
		for (int i = 0; i < numOfRocks; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(rockPositions[i].x, 0, rockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(rockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(rockScales[i] * mapSize / 20));
			uboRock[i].amb = .7f; uboRock[i].gamma = 180.0f; uboRock[i].sColor = glm::vec3(1.0f);
			uboRock[i].mvpMat = ViewPrj * GWorld;
			uboRock[i].mMat = GWorld;
			uboRock[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSRock[i].map(currentImage, &uboRock[i], sizeof(uboRock[i]), 0);
		}
	}

	void RenderTrees(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(treePositions[i].x, 0, treePositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(treeRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(treeScales[i] * mapSize / 20));
			uboTree[i].amb = .7f; uboTree[i].gamma = 180.0f; uboTree[i].sColor = glm::vec3(1.0f);
			uboTree[i].mvpMat = ViewPrj * GWorld;
			uboTree[i].mMat = GWorld;
			uboTree[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSTree[i].map(currentImage, &uboTree[i], sizeof(uboTree[i]), 0);
		}
	}

	void RenderTrees1(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees1; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree1Positions[i].x, 0, tree1Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree1Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree1Scales[i] * mapSize / 20));
			uboTree1[i].amb = .7f; uboTree1[i].gamma = 180.0f; uboTree1[i].sColor = glm::vec3(1.0f);
			uboTree1[i].mvpMat = ViewPrj * GWorld;
			uboTree1[i].mMat = GWorld;
			uboTree1[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSTree1[i].map(currentImage, &uboTree1[i], sizeof(uboTree1[i]), 0);
		}
	}

	void RenderTrees2(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees2; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree2Positions[i].x, 0, tree2Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree2Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree2Scales[i] * mapSize / 20));
			uboTree2[i].amb = .7f; uboTree2[i].gamma = 180.0f; uboTree2[i].sColor = glm::vec3(1.0f);
			uboTree2[i].mvpMat = ViewPrj * GWorld;
			uboTree2[i].mMat = GWorld;
			uboTree2[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSTree2[i].map(currentImage, &uboTree2[i], sizeof(uboTree2[i]), 0);
		}
	}

	void RenderTrees3(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees3; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree3Positions[i].x, 0, tree3Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree3Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree3Scales[i] * mapSize / 20));
			uboTree3[i].amb = .7f; uboTree3[i].gamma = 180.0f; uboTree3[i].sColor = glm::vec3(1.0f);
			uboTree3[i].mvpMat = ViewPrj * GWorld;
			uboTree3[i].mMat = GWorld;
			uboTree3[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSTree3[i].map(currentImage, &uboTree3[i], sizeof(uboTree3[i]), 0);
		}
	}

	void RenderTrees4(uint32_t currentImage)
	{
		for (int i = 0; i < numOfTrees4; i++)
		{
			GWorld = glm::translate(glm::mat4(1), glm::vec3(tree4Positions[i].x, 0, tree4Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree4Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree4Scales[i] * mapSize / 20));
			uboTree4[i].amb = .7f; uboTree4[i].gamma = 180.0f; uboTree4[i].sColor = glm::vec3(1.0f);
			uboTree4[i].mvpMat = ViewPrj * GWorld;
			uboTree4[i].mMat = GWorld;
			uboTree4[i].nMat = glm::inverse(glm::transpose(GWorld));
			DSTree4[i].map(currentImage, &uboTree4[i], sizeof(uboTree4[i]), 0);
		}
	}

	void RenderItems(uint32_t currentImage) {}

	void CalculateEnvironmentObjectsPositionsAndRotations()
	{
		srand(time(0));
		// Plants
		for (int i = 0; i < numOfPlants; i++)
		{
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			plantPositions[i] = { randX, randY };
			plantRotations[i] = randRot;
		}

		// Flowers
		for (int i = 0; i < numOfFlowers; i++)
		{
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
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
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			smallRockPositions[i] = { randX, randY };
			smallRockRotations[i] = randRot;
		}

		// Stumps
		for (int i = 0; i < numOfStumps; i++)
		{
			stumpScales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			stumpPositions[i] = { randX, randY };
			stumpRotations[i] = randRot;
			AddCollisionPoint(stumpPositions[i], stumpThreshold + (stumpScales[i] - 0.2f) * stumpThresholdCoefficient);
		}

		// Clouds
		for (int i = 0; i < numOfClouds; i++)
		{
			cloudScales[i] = (float)rand() / RAND_MAX / 6 + 0.15;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			randZ = (rand() < RAND_MAX / 2) ? mapSize / 3 : mapSize / 3 + 6;
			cloudPositions[i] = { randX, randY, randZ };
			cloudRotations[i] = randRot;
		}

		// Rocks
		for (int i = 0; i < numOfRocks; i++)
		{
			rockScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			rockPositions[i] = { randX, randY };
			rockRotations[i] = randRot;
			AddCollisionPoint(rockPositions[i], rockThreshold + (rockScales[i] - 0.05f) * rockThresholdCoefficient);
		}

		// Trees
		for (int i = 0; i < numOfTrees; i++)
		{
			treeScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			treePositions[i] = { randX, randY };
			treeRotations[i] = randRot;
			AddCollisionPoint(treePositions[i], treeThreshold + (treeScales[i] - 0.05f) * treeThresholdCoefficient);
		}

		// Trees1
		for (int i = 0; i < numOfTrees1; i++)
		{
			tree1Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			tree1Positions[i] = { randX, randY };
			tree1Rotations[i] = randRot;
			AddCollisionPoint(tree1Positions[i], treeThreshold + (tree1Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees2
		for (int i = 0; i < numOfTrees2; i++)
		{
			tree2Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			tree2Positions[i] = { randX, randY };
			tree2Rotations[i] = randRot;
			AddCollisionPoint(tree2Positions[i], treeThreshold + (tree2Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees3
		for (int i = 0; i < numOfTrees3; i++)
		{
			tree3Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			tree3Positions[i] = { randX, randY };
			tree3Rotations[i] = randRot;
			AddCollisionPoint(tree3Positions[i], treeThreshold + (tree3Scales[i] - 0.2f) * treeThresholdCoefficient);
		}

		// Trees4
		for (int i = 0; i < numOfTrees4; i++)
		{
			tree4Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
			randX = rand() % (mapSize - 1) - (mapSize / 2);
			randY = rand() % (mapSize - 1) - (mapSize / 2);
			randRot = rand() % (360 + 1);
			tree4Positions[i] = { randX, randY };
			tree4Rotations[i] = randRot;
			AddCollisionPoint(tree4Positions[i], treeThreshold + (tree4Scales[i] - 0.2f) * treeThresholdCoefficient);
		}
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