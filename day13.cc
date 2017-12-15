// Advent of Code 2017 day 13 solution
// Peter Kasting, Dec. 13, 2017

#include <algorithm>
#include <functional>
#include <iostream>
#include <regex>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Tokenizes |input|, a string representing a single scanner's depth and range,
// into a pair of (depth, range).
std::pair<int, int> ParseScanner(const std::string& input) {
  const std::regex regex("\\d+");
  std::sregex_iterator i(input.begin(), input.end(), regex);
  const int depth = std::stoi(i->str());
  ++i;
  return {depth, std::stoi(i->str())};
}

// Returns whether a packet beginning at |delay| would be detected by |scanner|.
bool Detected(int delay, const std::pair<int, int>& scanner) {
  // The period of a scanner is 2 * (range - 1).
  return ((scanner.first + delay) % (2 * (scanner.second - 1))) == 0;
}

// Returns the severity of a trip beginning with delay |delay|.
int GetSeverity(const std::vector<std::pair<int, int>>& scanners, int delay) {
  int severity = 0;
  for (const auto& i : scanners) {
    // The severity of getting detected by any single scanner is its depth times
    // its range.
    if (Detected(delay, i))
      severity += i.first * i.second;
  }
  return severity;
}

// Returns the minimum necessary delay for a packet to avoid detection by
// |scanners|.
int GetDelay(const std::vector<std::pair<int, int>>& scanners) {
  // This is slow, because it is O(scanners.size()) for each delay value.  It
  // seems like we ought to be able to do better -- ideally to have the whole
  // function run in O(scanners.size()) -- but I haven't figured out how.
  // It seemed promising to incrementing the delay by the least common multiple
  // of the periods of all successfully-bypassed scanners, instead of by 1, but
  // this skips some possible solutions.  Maybe something involving Euler's
  // Theorem or Chinese Remainder Theorem would be useful, but the fact that
  // each scanner's "window" is more than one time unit wide is making it hard
  // for me to see how to apply these.
  int delay = 0;
  while (std::any_of(scanners.begin(), scanners.end(),
                     std::bind(Detected, delay, std::placeholders::_1)))
    ++delay;
  return delay;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter program data; terminate with ctrl-z alone on a line."
            << std::endl;

  std::string input;
  std::vector<std::pair<int, int>> scanners;
  while (std::getline(std::cin, input))
    scanners.push_back(ParseScanner(input));

  if (kPart1)
    std::cout << "Severity: " << GetSeverity(scanners, 0) << std::endl;
  else
    std::cout << "Starting delay: " << GetDelay(scanners) << std::endl;

  return 0;
}