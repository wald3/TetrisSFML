#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>

#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace sc {
	struct Score {
		int Value;
	};

	inline void to_json(json& j, const Score& s) {
		j = { {"value", s.Value} };
	}

	inline void from_json(const json& j, Score& s) {
		j.at("value").get_to(s.Value);
	}

	inline bool sorter(const Score& s1, const Score& s2)
	{
		return s1.Value > s2.Value;
	}
}

namespace sv {
	class ScoreSaver {
	public:
		void saveScore(std::vector<sc::Score> scores);

		std::vector<sc::Score> loadScore();
	private:
		std::string fileName = "data.json";
	};
}
