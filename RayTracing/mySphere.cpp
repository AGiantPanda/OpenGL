#include "mySphere.h"



mySphere::mySphere():m_center(0),m_radius(0)
{
}

mySphere::mySphere(const glm::vec3 center, const float radius) : m_center(center), m_radius(radius)
{
}

mySphere::~mySphere()
{
}

void mySphere::setCenter(const glm::vec3 center)
{
	m_center = center;
}

void mySphere::setRadius(const float radius)
{
	m_radius = radius;
}

glm::vec3 mySphere::Center() const
{
	return m_center;
}

float mySphere::Radius() const
{
	return m_radius;
}

glm::vec3 mySphere::getNormal(const glm::vec3 point) const
{
	return glm::normalize(point - m_center);
}

Intersection mySphere::isIntersected(const Ray ray) const
{
	Intersection result = Intersection::noHit();
	glm::vec3 v = ray.Origin() - m_center;
	float _vd = glm::dot(v, ray.Direction());
	float _vd_sq = _vd * _vd;
	float _ac = glm::dot(v, v) - m_radius * m_radius;

	if (_vd <=0 && _vd_sq - _ac >= 0)
	{
		float t = -_vd - glm::sqrt(_vd_sq - _ac);
		result.distance = t;
		result.isHit = true;
		result.position = ray.getPoint(t);
		result.normal = glm::normalize(result.position - m_center);
	}

	return result;
}
