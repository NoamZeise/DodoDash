#ifndef GAME_HELPER_H
#define GAME_HELPER_H

#include <glm/glm.hpp>
#include <string>
#include <stdio.h>
#include <random>
#include <ctime>

namespace gh
{
	static bool colliding(glm::vec4 a, glm::vec4 b)
	{
		return  a.x < b.x + b.z &&
				a.x + a.z > b.x && 
				a.y < b.y + b.w &&
				a.y + a.w > b.y;
	}

	static bool aInB(glm::vec4 a, glm::vec4 b)
	{
		return a.x > b.x && a.x < b.x + b.z &&
				a.y > b.y && a.y < b.y + b.z;
	}

	static bool contains(glm::vec2 p, glm::vec4 r)
	{
		return r.x < p.x && p.x < r.x + r.z  && r.y < p.y && p.y < r.y + r.w;   
	}

	inline static bool exists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    } 
	}  

class Random
{
public:
	Random()
	{
		randomGen = std::mt19937(time(0));
		posReal = std::uniform_real_distribution<float>(0, 1);
		real = std::uniform_real_distribution<float>(-1, 1);
	}

	float Real()
	{
		return real(randomGen);
	}
	float PositiveReal()
	{
		return posReal(randomGen);
	}
private:
	std::mt19937 randomGen;
	std::uniform_real_distribution<float> posReal;
	std::uniform_real_distribution<float> real;
};
	


} //end namespace



#endif