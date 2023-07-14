void JungleExploration::CalculateEnvironmentObjectsPositionsAndRotations()
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
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		stumpPositions[i] = { randX, randY };
		stumpRotations[i] = randRot;
		AddCollisionPoint(stumpPositions[i], stumpThreshold + (stumpScales[i] - 0.2f) * stumpThresholdCoefficient);
	}

	// Clouds
	for (int i = 0; i < numOfClouds; i++)
	{
		cloudScales[i] = (float)rand() / RAND_MAX / 6 + 0.15;
		CalculateRandomPositionsRotations(-mapSize / 2 - mountainThreshold, mapSize / 2 + mountainThreshold);
		randZ = (rand() < RAND_MAX / 2) ? mapSize / 3 : mapSize / 3 + 6;
		cloudPositions[i] = { randX, randY, randZ };
		cloudRotations[i] = randRot;
	}

	// Rocks
	for (int i = 0; i < numOfRocks; i++)
	{
		rockScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		rockPositions[i] = { randX, randY };
		rockRotations[i] = randRot;
		AddCollisionPoint(rockPositions[i], rockThreshold + (rockScales[i] - 0.05f) * rockThresholdCoefficient);
	}

	// Trees
	for (int i = 0; i < numOfTrees; i++)
	{
		treeScales[i] = (float)rand() / RAND_MAX / 8 + 0.05;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		treePositions[i] = { randX, randY };
		treeRotations[i] = randRot;
		AddCollisionPoint(treePositions[i], treeThreshold + (treeScales[i] - 0.05f) * treeThresholdCoefficient);
	}

	// Trees1
	for (int i = 0; i < numOfTrees1; i++)
	{
		tree1Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		tree1Positions[i] = { randX, randY };
		tree1Rotations[i] = randRot;
		AddCollisionPoint(tree1Positions[i], treeThreshold + (tree1Scales[i] - 0.2f) * treeThresholdCoefficient);
	}

	// Trees2
	for (int i = 0; i < numOfTrees2; i++)
	{
		tree2Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		tree2Positions[i] = { randX, randY };
		tree2Rotations[i] = randRot;
		AddCollisionPoint(tree2Positions[i], treeThreshold + (tree2Scales[i] - 0.2f) * treeThresholdCoefficient);
	}

	// Trees3
	for (int i = 0; i < numOfTrees3; i++)
	{
		tree3Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
		tree3Positions[i] = { randX, randY };
		tree3Rotations[i] = randRot;
		AddCollisionPoint(tree3Positions[i], treeThreshold + (tree3Scales[i] - 0.2f) * treeThresholdCoefficient);
	}

	// Trees4
	for (int i = 0; i < numOfTrees4; i++)
	{
		tree4Scales[i] = (float)rand() / RAND_MAX / 6 + 0.2;
		CalculateRandomPositionsRotations(-mapSize / 2 + mountainThreshold, mapSize / 2 - mountainThreshold, true);
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

void JungleExploration::CalculateItemsPosition()
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

void JungleExploration::CheckItemPositionOverlap(int numOfObjects, float objectThreshold, float objectThresholdCoefficient, glm::vec2 objectPositions[], float objectScales[], bool& overlap)
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

void JungleExploration::CalculateRandomPositionsRotations(int start, int end, bool checkCenter)
{
	randX = rand() % (end - start + 1) + start;
	randY = rand() % (end - start + 1) + start;
	if (checkCenter)
	{
		while (true)
		{
			if (randX < 1 && randX > -1 && randY < 1 && randY > -1)
			{
				randX = rand() % (end - start + 1) + start;
				randY = rand() % (end - start + 1) + start;
			}
			else
			{
				break;
			}
		}
	}
	randRot = rand() % (360 + 1);
}