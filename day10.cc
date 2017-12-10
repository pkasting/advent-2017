// Advent of Code 2017 day 10 solution
// Peter Kasting, Dec. 10, 2017

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.
constexpr std::size_t kListLength = 256;  // Size of circular list of numbers.

// Returns {0, 1, 2, ..., kListLength - 1}.
std::vector<std::uint8_t> GenerateNumbers() {
  std::vector<std::uint8_t> numbers(kListLength);
  std::iota(numbers.begin(), numbers.end(), 0);
  return numbers;
}

// Converts |input| to a series of lengths to use to permute the circular list.
// In part 1, the input is parsed as comma-delimited lengths.  In part 2, each
// character of the input string is treated as a byte, whose ASCII value is a
// length.
std::vector<std::size_t> Tokenize(const std::string& input) {
  std::vector<std::size_t> lengths;
  if (kPart1) {
    // Split into comma-delimited lengths.
    std::istringstream stringstream(input);
    for (std::string length; std::getline(stringstream, length, ','); )
      lengths.push_back(static_cast<std::size_t>(std::stoi(length)));
  } else {
    // Treat each character as a length.
    for (char c : input)
      lengths.push_back(c);

    // Add hardcoded suffix.
    constexpr std::size_t kSuffix[5] = {17, 31, 73, 47, 23};
    lengths.insert(lengths.end(), std::begin(kSuffix), std::end(kSuffix));
  }
  return lengths;
}

// Reverses the sequence of |numbers| starting at |position| and having length
// |length|.  |numbers| is treated as a circular list, so positions/lengths off
// the end wrap around.
void Reverse(std::size_t position,
             std::size_t length,
             std::vector<std::uint8_t>* numbers) {
  // Returns the number in |numbers| at position |pos|, circularly.
  const auto Number = [numbers](std::size_t pos) -> std::uint8_t& {
    return (*numbers)[pos % kListLength];
  };

  // Reverse by pairwise-swapping the sequence ends, shrinking inward.
  for (std::size_t i = position; length > 1; ++i, length -= 2)
    std::swap(Number(i), Number(i + length - 1));
}

// For each length in |lengths|, reverses a subsequence of |numbers| of that
// length, then advances past the end of that subsequence by an ever-increasing
// skip value.  In part 2, repeats the processing of |lengths| 64 total times.
void ProcessLengths(const std::vector<std::size_t>& lengths,
                    std::vector<std::uint8_t>* numbers) {
  const int kRounds = kPart1 ? 1 : 64;
  std::size_t position = 0, skip_length = 0;
  for (int i = 0; i < kRounds; ++i) {
    for (std::size_t length : lengths) {
      Reverse(position, length, numbers);
      position = (position + length + skip_length) % kListLength;
      ++skip_length;
    }
  }
}

// Treating |numbers| as "the sparse hash", computes the "knot hash" by bitwise-
// XORing groups of 16 numbers, then printing them as a string of hex bytes.
std::string KnotFromSparseHash(const std::vector<std::uint8_t>& numbers) {
  // We could also manually convert to hex pretty easily (each nybble is just
  // "0123456789abcdef"[nybble]), but the code reads a little more clearly
  // using stringstreams and is at least as short.
  std::ostringstream output;
  output << std::hex << std::setfill('0');
  for (std::size_t pos = 0; pos < kListLength; ) {
    // XOR the next 16 numbers together.  We use an unsigned int instead of a
    // uint8_t because operator<<() below will ignore std::hex for uint8_t and
    // treat it as a char instead, so the output will look like garbage.
    unsigned int val = 0;
    for (std::size_t j = 0; (j < 16) && (pos < kListLength); ++j)
      val ^= numbers[pos++];

    // setw() resets after "<< val", and so must be done here instead of above.
    output << std::setw(2) << val;
  }
  return output.str();
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter length string: ";
  std::string input;
  std::getline(std::cin, input);

  std::vector<std::uint8_t> numbers(GenerateNumbers());
  ProcessLengths(Tokenize(input), &numbers);

  if (kPart1)
    std::cout << "Product: " << numbers[0] * numbers[1] << std::endl;
  else
    std::cout << "Knot hash: " << KnotFromSparseHash(numbers) << std::endl;

  return 0;
}