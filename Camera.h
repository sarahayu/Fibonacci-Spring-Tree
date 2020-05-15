#pragma once
#include <glm\glm.hpp>

class Camera
{
public:

	const glm::mat4 getProjView() const;
	const glm::mat4 getProjection() const;
	const glm::mat4 getView() const;
	const glm::mat3 getInvView() const;
	const glm::vec3 getPos() const;
	const glm::vec3 getFocusPos() const;

	void setProjection(const glm::mat4 &projection);
	void setView(const glm::mat4 &view);
	void setPos(const glm::vec3 &pos);
	void setFocusPos(const glm::vec3 &focusPos);

private:

	void setInvView();
	void recalculateView();

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat3 invView;

	glm::vec3 pos;
	glm::vec3 focus;
};