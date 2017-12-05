// Advent of Code 2017 day 1 solution
// Peter Kasting, Dec. 4, 2017

#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Tokenizes |input|, a string of digits, to a vector of individual digits.
std::vector<int> Tokenize(const std::string& input) {
  std::vector<int> digits;
  digits.reserve(input.length());  // Overkill but good practice.
  for (char c : input) {
    // Using std::stoi() is a bit heavyweight, but should nicely convert garbage
    // characters to zeroes.
    digits.push_back(std::stoi(std::string(1, c)));
  }
  return digits;
}

// Returns the sum of the digits in |digits| that match a particular later
// digit.  In part 1, this is the next digit; in part 2, the digit halfway
// around |digits|.
int SumMatchingDigits(const std::vector<int>& digits) {
  // Compute the increment to the digit we're interested in.
  const size_t size = digits.size();
  const size_t increment = kPart1 ? 1 : (size / 2);

  // Sum digits that match the one |increment| places away, treating |digits|
  // circularly.
  int sum = 0;
  for (size_t i = 0; i < size; ++i) {
    const int digit = digits[i];
    if (digit == digits[(i + increment) % size])
      sum += digit;
  }
  return sum;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter CAPTCHA: ";
  std::string input;
  std::cin >> input;

  std::cout << "Required sum is: " << SumMatchingDigits(Tokenize(input))
            << std::endl;
  return 0;
}
