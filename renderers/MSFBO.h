#pragma once
#include "FBO.h"

class MSFBO : public FBO
{
public:
	MSFBO();
	virtual ~MSFBO();

	virtual void rebuild(const sf::Vector2i &dimensions) override;

	virtual void bindTexture() override;

	void blitTexture();

private:
	FBO m_sample;
};