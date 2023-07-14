void JungleExploration::RenderCharacter(uint32_t currentImage)
{
	//std::cout << characterRotation.y;
	//glm::quat quaternion = glm::quat(glm::vec3(characterRotation.x, 0, characterRotation.z));
	glm::quat quaternionX = glm::angleAxis(-characterRotation.z, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::quat quaternionY = glm::angleAxis(glm::radians(characterRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat quaternionZ = glm::angleAxis(-characterRotation.x, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::quat combinedQuaternion = quaternionX * quaternionZ;
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0, 0.2f, 0));
	glm::mat4 rotationMatrix = glm::mat4_cast(combinedQuaternion);


	glm::mat4 coefficient = gameState == 0 ? glm::mat4(1) : World;
	GWorld = coefficient * translationMatrix * rotationMatrix;
	//* glm::mat4_cast(quaternion);
	//* glm::rotate(glm::mat4(1.0f), glm::radians(characterRotation.y), realNormY)
	//* glm::rotate(glm::mat4(1.0f), glm::radians(characterRotation.x), realNormX)
	//* glm::rotate(glm::mat4(1.0f), glm::radians(-characterRotation.z), realNormZ)
	;
	uboCharacter.visible = 1.0f;
	uboCharacter.amb = 1.0f;
	uboCharacter.gamma = 180.0f;
	uboCharacter.sColor = glm::vec3(1.0f);
	uboCharacter.mvpMat = ViewPrj * GWorld;
	uboCharacter.mMat = GWorld;
	uboCharacter.nMat = glm::inverse(glm::transpose(GWorld));
	DSCharacter.map(currentImage, &uboCharacter, sizeof(uboCharacter), 0);
}

void JungleExploration::RenderEnvironment(uint32_t currentImage)
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

void JungleExploration::RenderGround(uint32_t currentImage)
{
	for (int i = 0; i < 4; i++)
	{
		GWorld = glm::translate(glm::scale(glm::mat4(1), glm::vec3(mapSize / 2)), glm::vec3(groundPositions[i].x, 0, groundPositions[i].y));
		SetUboDs(currentImage, uboGround, DSGround, i);
	}
}

void JungleExploration::RenderPlants(uint32_t currentImage)
{
	for (int i = 0; i < numOfPlants; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(plantPositions[i].x, 0, plantPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(plantRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
		SetUboDs(currentImage, uboPlant, DSPlant, i, 0.7f);
	}
}

void JungleExploration::RenderFlowers(uint32_t currentImage)
{
	for (int i = 0; i < numOfFlowers; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(flowerPositions[i].x, 0, flowerPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(flowerRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.2f));
		SetUboDs(currentImage, uboFlower, DSFlower, i);
	}
}

void JungleExploration::RenderMountains(uint32_t currentImage)
{
	for (int i = 0; i < numOfMountains; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(mountainPositions[i].x, 0, mountainPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(mountainRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(mountainScales[i] * mapSize / 20));
		SetUboDs(currentImage, uboMountain, DSMountain, i);
	}
}

void JungleExploration::RenderSmallRocks(uint32_t currentImage)
{
	for (int i = 0; i < numOfSmallRocks; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(smallRockPositions[i].x, 0, smallRockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(smallRockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(.1f));
		SetUboDs(currentImage, uboSmallRock, DSSmallRock, i);
	}
}

void JungleExploration::RenderStumps(uint32_t currentImage)
{
	for (int i = 0; i < numOfStumps; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(stumpPositions[i].x, 0, stumpPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(stumpRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(stumpScales[i] * mapSize / 20));
		SetUboDs(currentImage, uboStump, DSStump, i);
	}
}

void JungleExploration::RenderClouds(uint32_t currentImage)
{
	for (int i = 0; i < numOfClouds; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(cloudPositions[i].x, cloudPositions[i].z, cloudPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(cloudRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(cloudScales[i] * mapSize / 20));
		SetUboDs(currentImage, uboCloud, DSCloud, i);
	}
}

void JungleExploration::RenderRocks(uint32_t currentImage)
{
	for (int i = 0; i < numOfRocks; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(rockPositions[i].x, 0, rockPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(rockRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(rockScales[i] * mapSize / 20));
		SetUboDs(currentImage, uboRock, DSRock, i);
	}
}

void JungleExploration::RenderTrees(uint32_t currentImage)
{
	for (int i = 0; i < numOfTrees; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(treePositions[i].x, 0, treePositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(treeRotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(treeScales[i] * mapSize / 20));
		SetUboDs(currentImage, uboTree, DSTree, i);
	}
}

void JungleExploration::RenderTrees1(uint32_t currentImage)
{
	for (int i = 0; i < numOfTrees1; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(tree1Positions[i].x, 0, tree1Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree1Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree1Scales[i] * mapSize / 20));
		SetUboDs(currentImage, uboTree1, DSTree1, i);
	}
}

void JungleExploration::RenderTrees2(uint32_t currentImage)
{
	for (int i = 0; i < numOfTrees2; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(tree2Positions[i].x, 0, tree2Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree2Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree2Scales[i] * mapSize / 20));
		SetUboDs(currentImage, uboTree2, DSTree2, i);
	}
}

void JungleExploration::RenderTrees3(uint32_t currentImage)
{
	for (int i = 0; i < numOfTrees3; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(tree3Positions[i].x, 0, tree3Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree3Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree3Scales[i] * mapSize / 20));
		SetUboDs(currentImage, uboTree3, DSTree3, i);
	}
}

void JungleExploration::RenderTrees4(uint32_t currentImage)
{
	for (int i = 0; i < numOfTrees4; i++)
	{
		GWorld = glm::translate(glm::mat4(1), glm::vec3(tree4Positions[i].x, 0, tree4Positions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(tree4Rotations[i]), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1), glm::vec3(tree4Scales[i] * mapSize / 20));
		SetUboDs(currentImage, uboTree4, DSTree4, i);
	}
}

void JungleExploration::RenderItems(uint32_t currentImage)
{
	for (int i = 0; i < numOfItems; i++)
	{
		if (isItemPicked[i])
		{
			//GWorld = glm::translate(glm::mat4(1), glm::vec3(mapSize * 4, 0, mapSize * 4));
			SetUboDs(currentImage, uboItem, DSItem, i, 0);
			continue;
		}
		GWorld = glm::translate(glm::mat4(1), glm::vec3(itemPositions[i].x, 0, itemPositions[i].y)) * glm::rotate(glm::mat4(1.0f), glm::radians(itemRotations[i]), glm::vec3(0, 1, 0));
		SetUboDs(currentImage, uboItem, DSItem, i, 1, 10, 180, { 1, 0, 0 });
	}
}

void JungleExploration::SetUboDs(uint32_t currentImage, UniformBufferObject ubo[], DescriptorSet DS[], int index, float visible, float amb, float gamma, glm::vec3 sColor)
{
	ubo[index].visible = visible;
	ubo[index].amb = amb;
	ubo[index].gamma = gamma;
	ubo[index].sColor = sColor;
	ubo[index].mvpMat = ViewPrj * GWorld;
	ubo[index].mMat = GWorld;
	ubo[index].nMat = glm::inverse(glm::transpose(GWorld));
	DS[index].map(currentImage, &ubo[index], sizeof(ubo[index]), 0);
}