// Advent of Code 2017 day 6 solution
// Peter Kasting, Dec. 5, 2017

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Tokenizes |input|, a series of whitespace-delimited ints, to a vector of
// ints.
std::vector<int> Tokenize(const std::string& input) {
  std::vector<int> tokenized;
  std::transform(std::istream_iterator<std::string>(std::istringstream(input)),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(tokenized),
                 [](const std::string& str) { return std::stoi(str); });
  return tokenized;
}

// Serializes |vec| to a string, using space as a delimiter.
std::string Serialize(const std::vector<int>& vec) {
  std::stringstream output;
  // We could use std::transform() and a std::ostring_iterator, but it's way
  // uglier.
  for (int i : vec)
    output << i << " ";  // I don't care about the extra trailing space.
  return output.str();
};

// Distributes the blocks in in max element of |banks| among all subsequent
// banks one at a time, circularly.
void Redistribute(std::vector<int>* banks) {
  auto bank = std::max_element(banks->begin(), banks->end());
  int blocks = *bank;
  for (*bank = 0; blocks--; ++*bank) {
    ++bank;
    if (bank == banks->end())
      bank = banks->begin();
  }
}

// Continually redistributes the blocks in |banks| until the same configuration
// recurs.  In part 1, returns the number of cycles required for this to occur;
// in part 2, returns the number of cycles spanned by the loop.
int CountCycles(std::vector<int> banks) {
  int cycles = 0;
  // Each bank configuration we've seen before, and on which cycle count.
  std::unordered_map<std::string, int> configs_seen;
  decltype(configs_seen)::iterator loop_point;

  // Records the current configuration in |configs_seen|.  Sets |new_config|
  // true if this is a new configuration; if not, points |loop_point| to the
  // entry containing the original cycle count for this configuration.
  bool new_config;
  auto Record = [&loop_point, &new_config, &configs_seen, banks, cycles]() {
    std::tie(loop_point, new_config) =
      configs_seen.insert(std::make_pair(Serialize(banks), cycles));
  };

  // Redistribute until we see a repeated configuration.
  for (Record(); new_config; Record()) {
    Redistribute(&banks);
    ++cycles;
  }

  return cycles - (kPart1 ? 0 : loop_point->second);
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter block counts: ";
  std::string input;
  std::getline(std::cin, input);

  std::cout << "Cycles: " << CountCycles(Tokenize(input)) << std::endl;
  return 0;
}
