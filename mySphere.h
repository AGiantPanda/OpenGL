#pragma once
#include <glm\glm.hpp>

#include "Ray.h"

class mySphere
{
public:
	mySphere();
	mySphere(const glm::vec3, const float);
	~mySphere();

	void setCenter(const glm::vec3);
	void setRadius(const float);
	glm::vec3 Center() const;
	float Radius() const;
	glm::vec3 getNormal(const glm::vec3) const;
	Intersection isIntersected(const Ray) const;

private:
	glm::vec3 m_center;
	float m_radius;
};
