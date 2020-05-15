#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>

const glm::mat4 Camera::getProjView() const
{
	return projection * view;
}

const glm::mat4 Camera::getProjection() const
{
	return projection;
}

const glm::mat4 Camera::getView() const
{
	return view;
}

const glm::mat3 Camera::getInvView() const
{
	return invView;
}

const glm::vec3 Camera::getPos() const
{
	return pos;
}

const glm::vec3 Camera::getFocusPos() const
{
	return focus;
}

void Camera::setProjection(const glm::mat4 & projection)
{
	this->projection = projection;
}

void Camera::setView(const glm::mat4 & view)
{
	this->view = view;
	setInvView();
}

void Camera::setPos(const glm::vec3 & pos)
{
	this->pos = pos;
	recalculateView();
}

void Camera::setFocusPos(const glm::vec3 & focusPos)
{
	this->focus = focusPos;
	recalculateView();
}

void Camera::setInvView()
{
	invView = glm::transpose(glm::inverse(glm::mat3(view)));
}

void Camera::recalculateView()
{
	glm::vec3 right = glm::normalize(glm::cross(focus - pos, { 0.f,1.f,0.f }));
	glm::vec3 up = glm::normalize(glm::cross(focus - pos, right)); up.y = -up.y;
	view = glm::lookAt(pos, focus, up);
	setInvView();
}
