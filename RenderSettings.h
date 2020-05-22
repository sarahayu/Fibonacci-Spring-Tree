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
	float ssaoRadius;
	bool autoRotate;
	bool multisampling;
	bool useShadows;
	bool useSSAO;
	bool useLighting;
	bool hideGUI;
	bool renderBackground;

	glm::vec3 sceneRotate;
	glm::vec3 sunPos;
	bool lines;
	float trueSunAzimuth;
};