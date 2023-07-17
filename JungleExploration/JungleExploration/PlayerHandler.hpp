bool canPickItem = false;
glm::vec3 characterRotation = { 0.0f, 90.0f , 0.0f };
glm::vec3 camPos = glm::vec3(0.0, 1.5, 0.0);
float camAlpha = 0.0f, camBeta = 0.0f;
glm::vec3 realNormX = { 1, 0, 0 };
glm::vec3 realNormY = { 0, 1, 0 };
glm::vec3 realNormZ = { 0, 0, 1 };
float movementAngle = 0;

void JungleExploration::Spectate()
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

void JungleExploration::PlayerJump(float deltaT, glm::vec3& m)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		isJumping = TRUE;
		//m.y = 1.0f;
	}
}

void JungleExploration::PlayerController(uint32_t currentImage)
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


#pragma region PlayerMovementRegion

	PlayerJump(deltaT, m);

	static glm::vec3 pos = startingPosition;
	static float yaw = 0, pitch = 0, roll = 0;
	static glm::quat rot = glm::quat(1, 0, 0, 0);
	static glm::mat4 ViewPrjOld = glm::mat4(1);
	static glm::mat4 camRy = glm::mat4(1);
	static glm::mat4 finalRy = glm::mat4(1);
	static float finalYaw = 0;
	static int counter = 0;

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
	if (m.x == 0)
	{
		characterRotation.x = 0;
	}
	else if (m.z == 0)
	{
		characterRotation.z = 0;
	}
	if (!isCollision)
	{
		pos += ux * MOVE_SPEED * m.x * deltaT;
		pos += uz * MOVE_SPEED * m.z * deltaT;
		//std::cout << pos.x << "=" << pos.z << " & ";
	}
	characterRotation += realNormX * MOVE_SPEED * m.x * deltaT;
	characterRotation += realNormZ * MOVE_SPEED * m.z * deltaT;
	if (characterRotation.x > 180)
	{
		characterRotation.x -= 360;
	}
	else if (characterRotation.x < -180)
	{
		characterRotation.x += 360;
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
	if (m.x == 0 && m.z == 0)
	{
		World = T * finalRy;
	}
	else
	{
		World = T * Ry;
		finalRy = Ry;
	}

	glm::vec3 c = glm::vec3(T * Ry * glm::vec4(0, camHeight + (camDist * sin(pitch)), camDist * cos(pitch), 1));
	glm::vec3 a = glm::vec3(T * Ry * glm::vec4(0, 0, 0, 1.0f)) + glm::vec3(0, camHeight, 0);

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
#pragma endregion

	PickItem(pos);
	ShowInteractionMessage(currentImage, pos);

}

void JungleExploration::PickItem(glm::vec3 pos)
{
	for (int i = 0; i < numOfItems; i++)
	{
		if ((pos.x < itemPositions[i].x + itemThreshold && pos.x > itemPositions[i].x - itemThreshold) &&
			(pos.z < itemPositions[i].y + itemThreshold && pos.z > itemPositions[i].y - itemThreshold))
		{
			if (glfwGetKey(window, GLFW_KEY_ENTER))
			{
				//text =
				//{
				//	{1, {"Spectate", "", "", ""}, 0, 0},
				//	{3, {"Main Game", "", "Total Items = " + std::to_string(1), ""}, 0, 0}
				//};
				//txt.updateText(&text);
				//txt.localCleanup();
				//txt.pipelinesAndDescriptorSetsCleanup();
				//txt.init(this, &text);
				//txt.pipelinesAndDescriptorSetsInit();
				//txt.populateCommandBuffer(commandBuffer, currentImage, currentScene);
				//RebuildPipeline();
				isItemPicked[i] = true;
				CheckEnding();
				break;
			}
		}
	}
}

void JungleExploration::ShowInteractionMessage(uint32_t currentImage, glm::vec3 pos)
{
	if (currentScene != 1 && gameState != 1)
	{
		uboInteractionMsg.visible = 0.0f;
		DSInteractionMsg.map(currentImage, &uboInteractionMsg, sizeof(uboInteractionMsg), 0);
		return;
	}
	canPickItem = false;
	for (int i = 0; i < numOfItems; i++)
	{
		if ((pos.x < itemPositions[i].x + itemThreshold && pos.x > itemPositions[i].x - itemThreshold) &&
			(pos.z < itemPositions[i].y + itemThreshold && pos.z > itemPositions[i].y - itemThreshold))
		{
			canPickItem = !isItemPicked[i];
			break;
		}
	}
	uboInteractionMsg.visible = canPickItem ? 1.0f : 0.0f;
	DSInteractionMsg.map(currentImage, &uboInteractionMsg, sizeof(uboInteractionMsg), 0);
}

void JungleExploration::CheckEnding()
{
	for (int i = 0; i < numOfItems; i++)
	{
		if (!isItemPicked[i])
			return;
	}
	currentScene = 2;
	RebuildPipeline();
}