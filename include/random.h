#ifndef GAME_RANDOM_H
#define GAME_RANDOM_H
#include <random>
#include <ctime>

class GameRandom
{
public:
	GameRandom()
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

#endif