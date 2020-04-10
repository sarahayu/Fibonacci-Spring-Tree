#pragma once
#include <glm\glm.hpp>

struct RenderSettings
{
	int fibStart;
	int iterations;
	float angle;
	float angleDecreaseFactor;
	float displacementAngle;
	float length;
	float lengthDecreaseFactor;
	float sunReach;
	float branchTaper;
	float rotate;
	float leafDensity;
	float sunAzimuth;
	float depthOfField;
	bool autoRotate;
	glm::vec3 sceneRotate;
};