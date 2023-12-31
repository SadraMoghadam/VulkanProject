#include "Starter.hpp"
#include "TextMaker.hpp"


struct GlobalUniformBlock
{
	alignas(16) glm::vec3 DlightDir;
	alignas(16) glm::vec3 DlightColor;
	alignas(16) glm::vec3 AmbLightColor;
	alignas(16) glm::vec3 eyePos;
};

struct MeshUniformBlock 
{
	alignas(4) float visible;
	alignas(4) float amb;
	alignas(4) float gamma;
	alignas(16) glm::vec3 sColor;
	alignas(16) glm::mat4 mvpMat;
	alignas(16) glm::mat4 mMat;
	alignas(16) glm::mat4 nMat;
};

struct OverlayUniformBlock 
{
	alignas(4) float visible;
};

struct VertexMesh
{
	glm::vec3 pos;
	glm::vec3 norm;
	glm::vec2 uv;
};

struct VertexOverlay
{
	glm::vec2 pos;
	glm::vec2 UV;
};


class JungleExploration : public BaseProject
{
protected:
	// Constant Parameters
	static const int mapScale = 10;
	static const int objectNumScale = 10;
	static const int mapSize = 5 * mapScale;
	static const int numOfPlants = 30 * objectNumScale;
	static const int numOfFlowers = 30 * objectNumScale;
	static const int numOfMountains = 7 * 4;
	static const int numOfSmallRocks = 30 * objectNumScale;
	static const int numOfStumps = 2 * objectNumScale;
	static const int numOfClouds = 2 * objectNumScale;
	static const int numOfRocks = 2 * objectNumScale;
	static const int numOfTrees = 1 * objectNumScale;
	static const int numOfTrees1 = 1 * objectNumScale;
	static const int numOfTrees2 = 1 * objectNumScale;
	static const int numOfTrees3 = 1 * objectNumScale;
	static const int numOfTrees4 = 1 * objectNumScale;
	static const int numOfItems = 5;
	static const int numOfSpikes = 2 * objectNumScale;
	static const int numOfCollisions = (numOfRocks + numOfStumps + numOfTrees * 5) * objectNumScale;

	// Descriptor Set Layouts
	DescriptorSetLayout DSLGubo, DSLToon, DSLToonPhong, DSLOverlay;

	// Vertex formats
	VertexDescriptor VMesh, VOverlay;

	// Pipelines
	Pipeline PToon, PToonPhong, POverlay;

	// Models
	Model<VertexMesh> MCharacter, MGround, MPlant, MFlower, MMountain, MSmallRock,
		MStump, MCloud, MRock, MTree, MTree1, MTree2, MTree3, MTree4, MItem, MSpike;
	Model<VertexOverlay> MStartPanel, MEndPanel, MLosePanel, MInteractionMsg;

	// Textures
	Texture TCharacter, TGround, TEnv, TEnv2, TItem, TStartPanel, TEndPanel, TLosePanel, TInteractionMsg;

	// Descriptor sets
	DescriptorSet DSGubo, DSCharacter, DSGround[4], DSPlant[numOfPlants], DSFlower[numOfFlowers],
		DSMountain[numOfMountains], DSSmallRock[numOfSmallRocks], DSStump[numOfStumps],
		DSCloud[numOfClouds], DSRock[numOfRocks], DSTree[numOfTrees], DSTree1[numOfTrees1],
		DSTree2[numOfTrees2], DSTree3[numOfTrees3], DSTree4[numOfTrees4], DSItem[numOfItems],
		DSSpike[numOfSpikes], DSStartPanel, DSEndPanel, DSLosePanel, DSInteractionMsg;

	// Uniform Blocks
	GlobalUniformBlock gubo;
	MeshUniformBlock uboCharacter, uboGround[4], uboPlant[numOfPlants], uboFlower[numOfFlowers],
		uboMountain[numOfMountains], uboSmallRock[numOfSmallRocks], uboStump[numOfStumps],
		uboCloud[numOfClouds], uboRock[numOfRocks], uboTree[numOfTrees], uboTree1[numOfTrees1],
		uboTree2[numOfTrees2], uboTree3[numOfTrees3], uboTree4[numOfTrees4], uboItem[numOfItems],
		uboSpike[numOfSpikes];
	OverlayUniformBlock uboStartPanel, uboEndPanel, uboLosePanel, uboInteractionMsg;

	// Text
	TextMaker txt;
	int numOfItemsInMap = numOfItems;
	std::vector<SingleText> text =
	{
		{1, {"Spectate", "", "", ""}, 0, 0},
		{4, {"Main Game", "Total Items = " + std::to_string(numOfItemsInMap), "", "Find the precious gems and avoid the yellow spikes"}, 0, 0}
	};

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
	//float treeRotations[numOfTrees];
	float treeScales[numOfTrees];
	glm::vec2 tree1Positions[numOfTrees1];
	float tree1Rotations[numOfTrees1];
	float tree1Scales[numOfTrees1];
	glm::vec2 tree2Positions[numOfTrees2];
	//float tree2Rotations[numOfTrees2];
	float tree2Scales[numOfTrees2];
	glm::vec2 tree3Positions[numOfTrees3];
	float tree3Rotations[numOfTrees3];
	float tree3Scales[numOfTrees3];
	glm::vec2 tree4Positions[numOfTrees4];
	float tree4Rotations[numOfTrees4];
	float tree4Scales[numOfTrees4];
	glm::vec2 itemPositions[numOfItems];
	float itemRotations[numOfItems];
	bool isItemPicked[numOfItems];
	glm::vec2 spikePositions[numOfSpikes];
	float spikeRotations[numOfSpikes];
	float spikeScales[numOfSpikes];

	//Jump params
	bool isJumping = FALSE;
	float VJumpIni = .5f;
	float VJump = VJumpIni;
	float g = -1.1f;

	//Animation params
	bool pickAnimation = false;
	float VpickJumpIni = 1.5f;
	float VpickJump = VpickJumpIni;
	float gAnimation = -10.0f;
	int animationCounter = 0;

	// Collision Parameters
	bool xCollision = false, yCollision = false, isCollision = false;
	int thresholdIndex = 0;
	float mountainThreshold = 7.8f;
	float mountainThresholdCoefficient = 10;
	std::tuple<glm::vec2, float> collisionsInfo[numOfCollisions];
	float rockThreshold = 0.4f;
	float rockThresholdCoefficient = 8;
	float stumpThreshold = 0.4f;
	float stumpThresholdCoefficient = 1;
	float treeThreshold = 0.3f;
	float treeThresholdCoefficient = 1;
	float itemThreshold = 0.5f;
	float spikeThreshold = 0.7f;

	// Other Parameters
	int gameState = 1;
	int currentScene = 0;
	float Ar;
	glm::mat4 World, ViewPrj, GWorld;


	void setWindowParameters()
	{
		windowWidth = 1920;
		windowHeight = 1080;
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
		uniformBlocksInPool = 2 + 4 + numOfPlants * 2 + numOfFlowers * 2 + numOfMountains * 2 + numOfSmallRocks * 2 +
			numOfStumps * 2 + numOfClouds * 2 + numOfRocks * 2 + numOfTrees * 2 * 5 + numOfItems * 2 + numOfSpikes * 2 + 4 + 1;
		texturesInPool = 9 + 1;
		setsInPool = 2 + 4 + numOfPlants + numOfFlowers + numOfMountains + numOfSmallRocks +
			numOfStumps + numOfClouds + numOfRocks + numOfTrees * 5 + numOfItems + numOfSpikes + 4 + 1;
	}

	void localInit()
	{
		// Initializing Descriptor Set Layouts
		DSLGubo.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS}
			});
		DSLToon.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLToonPhong.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
			});
		DSLOverlay.init(this, {
			{0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
			{1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
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
		VOverlay.init(this, {
				  {0, sizeof(VertexOverlay), VK_VERTEX_INPUT_RATE_VERTEX}
			}, {
			  {0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, pos),
					 sizeof(glm::vec2), OTHER},
			  {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexOverlay, UV),
					 sizeof(glm::vec2), UV}
			});

		// Initializing Pipelines
		PToon.init(this, &VMesh, "shaders/ToonVert.spv", "shaders/ToonFrag.spv", { &DSLGubo, &DSLToon });
		PToonPhong.init(this, &VMesh, "shaders/ToonPhongVert.spv", "shaders/ToonPhongFrag.spv", { &DSLGubo, &DSLToonPhong });
		POverlay.init(this, &VOverlay, "shaders/OverlayVert.spv", "shaders/OverlayFrag.spv", { &DSLOverlay });
		POverlay.setAdvancedFeatures(VK_COMPARE_OP_LESS_OR_EQUAL, VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, false);

		// Initializing Models
		//MCharacter.init(this, &VMesh, "Models/character.obj", OBJ);
		CreateSphereMesh(MCharacter.vertices, MCharacter.indices);
		MCharacter.initMesh(this, &VMesh);
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
		MItem.init(this, &VMesh, "Models/item.mgcg", MGCG);
		MSpike.init(this, &VMesh, "Models/spike.mgcg", MGCG);
		// Overlay Models
		CreateOverlayMesh(MStartPanel.vertices, MStartPanel.indices);
		MStartPanel.initMesh(this, &VOverlay);
		CreateOverlayMesh(MEndPanel.vertices, MEndPanel.indices);
		MEndPanel.initMesh(this, &VOverlay);
		CreateOverlayMesh(MLosePanel.vertices, MLosePanel.indices);
		MLosePanel.initMesh(this, &VOverlay);
		CreateOverlayMesh(MInteractionMsg.vertices, MInteractionMsg.indices, -0.7f, 0.7f, 0.8f, 0.5f);
		MInteractionMsg.initMesh(this, &VOverlay);

		// Initializing Textures
		TCharacter.init(this, "textures/Character.jpg");
		TGround.init(this, "textures/Ground.png");
		TEnv.init(this, "textures/Texture_01.jpg");
		TEnv2.init(this, "textures/Terrain-Texture_2.png");
		TItem.init(this, "textures/Textures_Dungeon.png");
		TStartPanel.init(this, "textures/Menu.jpg");
		TEndPanel.init(this, "textures/Ending.jpg");
		TLosePanel.init(this, "textures/LosePanel.png");
		TInteractionMsg.init(this, "textures/InteractMsg.png");

		txt.init(this, &text, -0.95, -0.95, 1.0 / 1200.0, 1.0 / 800.0);

		// Init local variables
		for (int i = 0; i < numOfItems; i++)
		{
			isItemPicked[i] = false;
		}
		CalculateEnvironmentObjectsPositionsAndRotations();
	}

	void pipelinesAndDescriptorSetsInit()
	{
		// Creating Pipelines
		PToon.create();
		PToonPhong.create();
		POverlay.create();

		// Defining the Descriptor Sets
		DSGubo.init(this, &DSLGubo, {
					{0, UNIFORM, sizeof(GlobalUniformBlock), nullptr}
			});
		DSCharacter.init(this, &DSLToon, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TCharacter}
			});
		for (int i = 0; i < 4; i++)
		{
			DSGround[i].init(this, &DSLToonPhong, {
						{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
						{1, TEXTURE, 0, &TGround}
				});
		}
		for (int i = 0; i < numOfPlants; i++)
		{
			DSPlant[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfFlowers; i++)
		{
			DSFlower[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfMountains; i++)
		{
			DSMountain[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfSmallRocks; i++)
		{
			DSSmallRock[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfStumps; i++)
		{
			DSStump[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfClouds; i++)
		{
			DSCloud[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfRocks; i++)
		{
			DSRock[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees; i++)
		{
			DSTree[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv2},
				});
		}
		for (int i = 0; i < numOfTrees1; i++)
		{
			DSTree1[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees2; i++)
		{
			DSTree2[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees3; i++)
		{
			DSTree3[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfTrees4; i++)
		{
			DSTree4[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEnv},
				});
		}
		for (int i = 0; i < numOfItems; i++)
		{
			DSItem[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TItem},
				});
		}
		for (int i = 0; i < numOfSpikes; i++)
		{
			DSSpike[i].init(this, &DSLToon, {
					{0, UNIFORM, sizeof(MeshUniformBlock), nullptr},
					{1, TEXTURE, 0, &TItem},
				});
		}
		DSStartPanel.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TStartPanel}
			});
		DSEndPanel.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TEndPanel}
			});
		DSLosePanel.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TLosePanel}
			});
		DSInteractionMsg.init(this, &DSLOverlay, {
					{0, UNIFORM, sizeof(OverlayUniformBlock), nullptr},
					{1, TEXTURE, 0, &TInteractionMsg}
			});

		txt.pipelinesAndDescriptorSetsInit();

	}

	void pipelinesAndDescriptorSetsCleanup()
	{
		// Cleanup Pipelines
		PToon.cleanup();
		PToonPhong.cleanup();

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
		for (int i = 0; i < numOfSpikes; i++)
			DSSpike[i].cleanup();
		DSStartPanel.cleanup();
		DSEndPanel.cleanup();
		DSLosePanel.cleanup();
		DSInteractionMsg.cleanup();

		txt.pipelinesAndDescriptorSetsCleanup();
	}

	void localCleanup()
	{
		// Cleanup Textures
		TCharacter.cleanup();
		TGround.cleanup();
		TEnv.cleanup();
		TEnv2.cleanup();
		TItem.cleanup();
		TStartPanel.cleanup();
		TEndPanel.cleanup();
		TLosePanel.cleanup();
		TInteractionMsg.cleanup();

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
		MSpike.cleanup();
		MStartPanel.cleanup();
		MEndPanel.cleanup();
		MLosePanel.cleanup();
		MInteractionMsg.cleanup();

		// Cleanup Descriptor Set Layouts
		DSLGubo.cleanup();
		DSLToon.cleanup();
		DSLToonPhong.cleanup();
		DSLOverlay.cleanup();

		// Destroying the Pipeline
		PToon.destroy();
		PToonPhong.destroy();
		POverlay.destroy();

		txt.localCleanup();

	}

	void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage)
	{
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

		MSpike.bind(commandBuffer);
		for (int i = 0; i < numOfSpikes; i++) {
			DSSpike[i].bind(commandBuffer, PToon, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MSpike.indices.size()), 1, 0, 0, 0);
		}


		DSGubo.bind(commandBuffer, PToonPhong, 0, currentImage);

		// Ground
		PToonPhong.bind(commandBuffer);
		MGround.bind(commandBuffer);
		for (int i = 0; i < 4; i++)
		{
			DSGround[i].bind(commandBuffer, PToonPhong, 1, currentImage);
			vkCmdDrawIndexed(commandBuffer,
				static_cast<uint32_t>(MGround.indices.size()), 1, 0, 0, 0);
		}

		// Overlays
		POverlay.bind(commandBuffer);
		MStartPanel.bind(commandBuffer);
		DSStartPanel.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MStartPanel.indices.size()), 1, 0, 0, 0);

		MEndPanel.bind(commandBuffer);
		DSEndPanel.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MEndPanel.indices.size()), 1, 0, 0, 0);

		MLosePanel.bind(commandBuffer);
		DSLosePanel.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MLosePanel.indices.size()), 1, 0, 0, 0);

		MInteractionMsg.bind(commandBuffer);
		DSInteractionMsg.bind(commandBuffer, POverlay, 0, currentImage);
		vkCmdDrawIndexed(commandBuffer,
			static_cast<uint32_t>(MInteractionMsg.indices.size()), 1, 0, 0, 0);

		txt.populateCommandBuffer(commandBuffer, currentImage, gameState, currentScene);
	}

	void updateUniformBuffer(uint32_t currentImage)
	{
		static bool debounce = false;
		static int curDebounce = 0;

		// Standard procedure to quit when the ESC key is pressed
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}


		if (currentScene == 0 && glfwGetKey(window, GLFW_KEY_ENTER))
		{
			currentScene++;
			RebuildPipeline();
		}

		if (currentScene != 1 || gameState != 1)
		{
			uboInteractionMsg.visible = 0.0f;
			DSInteractionMsg.map(currentImage, &uboInteractionMsg, sizeof(uboInteractionMsg), 0);
		}

		uboStartPanel.visible = (currentScene == 0) ? 1.0f : 0.0f;
		DSStartPanel.map(currentImage, &uboStartPanel, sizeof(uboStartPanel), 0);

		uboEndPanel.visible = (currentScene == 2) ? 1.0f : 0.0f;
		DSEndPanel.map(currentImage, &uboEndPanel, sizeof(uboEndPanel), 0);

		uboLosePanel.visible = (currentScene == 3) ? 1.0f : 0.0f;
		DSLosePanel.map(currentImage, &uboLosePanel, sizeof(uboLosePanel), 0);



		if (glfwGetKey(window, GLFW_KEY_M)) {
			if (!debounce) {
				debounce = true;
				curDebounce = GLFW_KEY_M;
				gameState = (gameState + 1) % 2;
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
		if (currentScene == 1)
		{
			switch (gameState)
			{
			case 0:
				Spectate();
				break;
			case 1:
				PlayerController(currentImage);
				break;
			default:
				break;

			}
		}

		gubo.DlightDir = glm::vec3(cos(glm::radians(135.0f)) * cos(glm::radians(210.0f)), sin(glm::radians(135.0f)), cos(glm::radians(135.0f)) * sin(glm::radians(210.0f)));
		gubo.DlightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		gubo.AmbLightColor = glm::vec3(0.2f);
		gubo.eyePos = glm::vec3(100.0, 100.0, 100.0);

		DSGubo.map(currentImage, &gubo, sizeof(gubo), 0);

		
		RenderCharacter(currentImage);
		RenderEnvironment(currentImage);
	}

	void Spectate();

	void PlayerJump(float deltaT, glm::vec3& m);

	void PlayerController(uint32_t currentImage);

	void PickAnimation(float deltaT, glm::vec3& pos);

	void PickItem(glm::vec3 pos);

	void ShowInteractionMessage(uint32_t currentImage, glm::vec3 pos);

	void CheckEnding();

	void CheckLose(glm::vec3 pos);

	void MapBorderCollisionHandler(glm::vec3& pos, glm::vec3& nextPos);

	void CollisionChecker(glm::vec3 nextPos);

	void AddCollisionPoint(glm::vec2 position, float threshold);

	void RenderCharacter(uint32_t currentImage);

	void RenderEnvironment(uint32_t currentImage);

	void RenderGround(uint32_t currentImage);

	void RenderPlants(uint32_t currentImage);

	void RenderFlowers(uint32_t currentImage);

	void RenderMountains(uint32_t currentImage);

	void RenderSmallRocks(uint32_t currentImage);

	void RenderStumps(uint32_t currentImage);

	void RenderClouds(uint32_t currentImage);

	void RenderRocks(uint32_t currentImage);

	void RenderTrees(uint32_t currentImage);

	void RenderTrees1(uint32_t currentImage);

	void RenderTrees2(uint32_t currentImage);

	void RenderTrees3(uint32_t currentImage);

	void RenderTrees4(uint32_t currentImage);

	void RenderItems(uint32_t currentImage);

	void RenderSpikes(uint32_t currentImage);

	void SetUboDs(uint32_t currentImage, MeshUniformBlock ubo[], DescriptorSet DS[], int index, float visible = 1.0f, float amb = 1.0f, 
		float gamma = 80.0f, glm::vec3 sColor = glm::vec3(1.0f));

	void CalculateEnvironmentObjectsPositionsAndRotations();

	void CalculateItemsPosition();

	void CheckItemPositionOverlap(int numOfObjects, float objectThreshold, float objectThresholdCoefficient, glm::vec2 objectPositions[], 
		float objectScales[], bool& overlap);

	void CalculateRandomPositionsRotations(int start = -mapSize / 2 + 1, int end = mapSize / 2 - 1, bool checkCenter = false);

	void CreateSphereMesh(std::vector<VertexMesh>& vDef, std::vector<uint32_t>& vIdx);

	void CreateOverlayMesh(std::vector<VertexOverlay>& vDef, std::vector<uint32_t>& vIdx, float left = -1.0f, float right = 1.0f, float top = 1.0f, float bottom = -1.0f);
};


#include "MeshGenerator.hpp"
#include "PlayerHandler.hpp"
#include "CollisionHandler.hpp"
#include "Renderer.hpp"
#include "ObjectsSpecificationsCalculator.hpp"

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