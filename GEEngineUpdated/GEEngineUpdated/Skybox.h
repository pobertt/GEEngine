#pragma once

#include "maths.h"

class Skybox {
public:
	float rotationAngle;
	float rotationSpeed;

	Skybox()// Constructor
	{
		rotationAngle = 0.0f;
		rotationSpeed = 0.2f;
	}

	// This replaces the free function we wrote earlier
	Matrix GetModelMatrix(const Vec3& playerPos, float dt) {
		rotationAngle += rotationSpeed * dt;
		if (rotationAngle > 6.28318f) { // 2*PI
			rotationAngle -= 6.28318f;
		}

		// 2. Create Rotation Matrix (Spin around Y)
		Matrix rotation;
		rotation.rotAroundY(rotationAngle);

		// 3. Create Translation Matrix (Move to Player)
		Matrix translation;
		translation.identity();
		translation.translation(playerPos);

		// 4. Combine (Rotate first, then Move)
		return translation.multiply(rotation);
	}
};