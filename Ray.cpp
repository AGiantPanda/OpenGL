#include "Ray.h"



Ray::Ray():m_ori(glm::vec3()),m_dir(glm::vec3())
{
}

Ray::Ray(const glm::vec3 origin, const glm::vec3 direction):m_ori(origin),m_dir(glm::normalize(direction))
{
}

Ray::~Ray()
{
}

void Ray::setOrigin(const glm::vec3 origin)
{
	m_ori = origin;
}

void Ray::setDirection(const glm::vec3 direction)
{
	m_dir = glm::normalize(direction);
}

glm::vec3 Ray::Origin() const
{
	return m_ori;
}

glm::vec3 Ray::Direction() const
{
	return m_dir;
}

glm::vec3 Ray::getPoint(const float t) const
{
	return m_ori + t * m_dir;
}
