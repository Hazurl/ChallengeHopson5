#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>

std::string to_upper(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}

void readWords(std::vector<std::string>& words) {
	std::ifstream wordslist("wordslist.txt");

	if(!wordslist) {
		std::cout << "Cannot open wordslist.txt" << std::endl;
		std::cin.get();
		exit(1);
	}

	std::cout << "Please wait..." << std::endl;
	words.clear();
	words.reserve(470000);
	while(!wordslist.eof()) {
		std::string line;
		std::getline(wordslist, line);
		if(line != "")
			words.push_back(to_upper(line));
	}
}

std::string choose(std::vector<std::string> const& s, std::vector<std::string>::const_iterator const& end) {
	std::default_random_engine gen(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<int> dis(0, std::distance(s.begin(), end) - 1);
	auto r = dis(gen);
	return s[r];
}

std::string choose(std::vector<std::string> const& s) {
	return choose(s, s.end());
}

std::string askWord() {
	std::cout << "Guess the words: ";
	std::string guess;
	std::cin.ignore();
	std::cin >> guess;
	return to_upper(guess);
}

bool play_with_user(std::string const& word, bool easy_mode) {
	std::string guess = askWord();

	bool not_found = false;
	int right = 0;
	for(unsigned int i = 0; i < word.size(); i++) {
		if(i < guess.size() && guess[i] == word[i])
			easy_mode && (std::cout << word[i]), right++;
		else
			easy_mode && (std::cout << '_'), not_found = true;
	}
	std::cout << " " << right << "/" << word.size() << std::endl;
	return not_found;
}

void play(std::string const& word, bool easy_mode = false, const int step_max = 5) {
	std::cout << "You have to guess a word of " << word.size() << " letters in " << step_max << " max steps !" << std::endl;
	int i = step_max + 1;
	while((--i) && play_with_user(word, easy_mode));

	if(i) {
		std::cout << "Good job, you found the word in " << (step_max - i + 1) << " steps !" << std::endl;
	} else {
		std::cout << "You failed, the word was \"" << word << "\" !" << std::endl;
	}
}

int right_letter(std::string const& ref, std::string const& g) {
	int right = 0;
	for(unsigned int i = 0; i < ref.size(); i++)
		if(i < g.size() && g[i] == ref[i])
			right++;
	return right;
}

void computer(std::vector<std::string>& words, std::string const& word, int step) {
	unsigned int size = word.size();
	auto w_end = std::remove_if(words.begin(), words.end(), [size](std::string const& s) {
		return s.size() != size;
	});
	bool not_found = true;
	int i = step + 1;
	while(not_found && (--i)) {
		std::string guess = choose(words, w_end);
		int right = right_letter(guess, word);
		not_found = right != word.size();
		if(not_found) {
			w_end = std::remove_if(words.begin(), w_end, [&guess, right] (std::string const& s) {
				return right_letter(guess, s) != right || guess == s;
			});
		}
		std::cout << "Computer try : " << guess << " " << right << "/" << word.size() << std::endl;
	}


	if(i) {
		std::cout << "The computer found the word in " << (step - i) << " steps !" << std::endl;
	} else {
		std::cout << "The computer failed, the word was \"" << word << "\" !" << std::endl;
		std::cout << "Remaining words: " << std::endl;
		int __i = 15;
		for(auto it = words.begin(); __i && it != w_end; ++it, --__i) {
			std::cout << "\t> " << *it << std::endl;
		}
		if(i == 0)
			std::cout << "\t... and " << std::distance(words.begin(), w_end) << " others" << std::endl;
	}
}

int main(int argc, char** argv) {
	bool easy_mode = false;
	bool self_mode = false;

	for(; argc-- > 1; ) {
		std::string arg = argv[argc];
		if(arg == "-self")
			self_mode = true;
		else if(arg == "-easy")
			easy_mode = true;
		else {
			std::cout << "Hopson challenge #5, by Hazurl" << std::endl;
			std::cout << "\t-easy : increase the number of maximum attempts" << std::endl;
			std::cout << "\t-self : if you want to see somebody good at this game" << std::endl;
			return 0;
		}
	}

	std::vector<std::string> words;
	readWords(words);

	std::string word = choose(words);
	int step = word.size() * (easy_mode ? 2 : 1);
	if(step < 5) step = 5;
	if(step > 12) step = 12;
	if(self_mode)
		computer(words, word, step);
	else
		play(word, easy_mode, step);
	return 0;
}