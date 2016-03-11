#pragma once
#include <glm\glm.hpp>

class Ray
{
public:
	Ray();
	Ray(const glm::vec3, const glm::vec3);
	~Ray();

	void setOrigin(const glm::vec3);
	void setDirection(const glm::vec3);
	glm::vec3 Origin() const;
	glm::vec3 Direction() const;
	glm::vec3 getPoint(const float) const;

private:
	glm::vec3 m_ori;
	glm::vec3 m_dir;
};

struct Intersection {
	float distance;
	bool isHit;
	glm::vec3 position;
	glm::vec3 normal;
	static inline Intersection noHit() { return Intersection(); }
};
