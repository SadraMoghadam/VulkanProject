void JungleExploration::MapBorderCollisionHandler(glm::vec3& pos, glm::vec3& nextPos)
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

void JungleExploration::CollisionChecker(glm::vec3 nextPos)
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

void JungleExploration::AddCollisionPoint(glm::vec2 position, float threshold)
{
	std::tuple<glm::vec2, float> collisionInfo(position, threshold);
	collisionsInfo[thresholdIndex++] = collisionInfo;
}