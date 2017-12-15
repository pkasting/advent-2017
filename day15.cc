// Advent of Code 2017 day 15 solution
// Peter Kasting, Dec. 15, 2017

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <regex>
#include <string>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Reads a line of input and converts the first number in it to a uint64_t.
std::uint64_t GetStartingValue() {
  std::string input;
  std::getline(std::cin, input);
  std::smatch match;
  const std::regex regex("\\d+");
  std::regex_search(input, match, regex);
  return std::stoi(match.str());
}

// Given a generator with factor |factor|, updates the starting |val| to the
// next non-ignored value.  In part 1, no values are ignored; in part 2, values
// not a multiple of |multiple| are ignored.
void Generate(std::uint64_t factor,
              std::uint64_t multiple,
              std::uint64_t* val) {
  constexpr std::uint64_t kDivisor = 0x7fffffff;
  do
    *val = (*val * factor) % kDivisor;
  while (!kPart1 && (*val % multiple));
}

// Returns the number of matches in a sample of generator runs with starting
// values |a| and |b|.  The sample size in part 1 is 40,000,000; in part 2 it is
// 5,000,000.  The factors and multiples for the generators are hardcoded from
// the problem specification.  Output values are considered to match when their
// least significant 16 bits are equal.
std::size_t CountMatches(std::uint64_t a, std::uint64_t b) {
  std::size_t count = 0;
  constexpr std::size_t kSampleSize = kPart1 ? 40000000 : 5000000;
  for (std::size_t i = 0; i < kSampleSize; ++i) {
    // Update generators A and B to their next values.
    constexpr std::uint64_t kFactorA = 16807;
    constexpr std::uint64_t kMultipleA = 4;
    Generate(kFactorA, kMultipleA, &a);
    constexpr std::uint64_t kFactorB = 48271;
    constexpr std::uint64_t kMultipleB = 8;
    Generate(kFactorB, kMultipleB, &b);

    // See if the bottom 16 bits match.
    if ((a & 0xffff) == (b & 0xffff))
      ++count;
  }
  return count;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter starting values." << std::endl;

  // Note: Inlining these calls into the statement below is dangerous because
  // the order of evaluation of the parameters to CountMatches() is undefined.
  const std::uint64_t a = GetStartingValue();
  const std::uint64_t b = GetStartingValue();

  std::cout << "Judge's count: " << CountMatches(a, b) << std::endl;

  return 0;
}