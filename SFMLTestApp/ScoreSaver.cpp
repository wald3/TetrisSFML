#include "ScoreSaver.h"

void sv::ScoreSaver::saveScore(std::vector<sc::Score> scores)
{
	std::ofstream output;
	output.open(this->fileName);
	output << std::setw(4) << json(scores) << std::endl;
	output.close();
}

std::vector<sc::Score> sv::ScoreSaver::loadScore()
{
    std::ifstream input;
    json j;

    input.open(this->fileName);
    input >> j;
    input.close();

    std::vector<sc::Score> s = j;
    return s;
}
