// Advent of Code 2017 day 10 solution
// Peter Kasting, Dec. 10, 2017

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

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

// Computes a "sparse hash" using |lengths|.
std::vector<std::uint8_t> SparseHash(const std::vector<std::size_t>& lengths) {
  // Initial vector is the series {0, 1, 2, ..., 255}.
  constexpr std::size_t kListLength = 256;
  std::vector<std::uint8_t> hash(kListLength);
  std::iota(hash.begin(), hash.end(), 0);

  // Reverses the subsequence of |hash| at [pos, pos + len), treating |hash|
  // circularly.
  const auto Reverse = [&hash, kListLength](std::size_t pos, std::size_t len) {
    // Reverse by pairwise-swapping the sequence ends, shrinking inward.
    for (std::size_t i = pos; len > 1; ++i, len -= 2)
      std::swap(hash[i % kListLength], hash[(i + len - 1) % kListLength]);
  };

  // Permute for 1 or 64 rounds (depending on the part), preserving the position
  // and skip length between rounds.
  const int kRounds = kPart1 ? 1 : 64;
  std::size_t position = 0, skip_length = 0;
  for (int i = 0; i < kRounds; ++i) {
    for (std::size_t length : lengths) {
      Reverse(position, length);
      position = (position + length + skip_length) % kListLength;
      ++skip_length;
    }
  }

  return hash;
}

// Computes the "knot hash" of the provided |sparse_hash| by bitwise-XORing
// groups of 16 numbers, then concatenating the bit representations of the
// results.
std::string KnotHash(const std::vector<std::uint8_t>& sparse_hash) {
  // We could also manually convert to hex pretty easily (each nybble is just
  // "0123456789abcdef"[nybble]), but the code reads a little more clearly
  // using stringstreams and is at least as short.
  std::ostringstream output;
  output << std::hex << std::setfill('0');
  for (auto i = sparse_hash.cbegin(); i != sparse_hash.cend(); ) {
    // XOR the next (up to) 16 numbers together.
    constexpr std::size_t kValuesToXor = 16;
    const size_t remaining_size =
        static_cast<std::size_t>(std::distance(i, sparse_hash.end()));
    const auto end = std::next(i, std::min(kValuesToXor, remaining_size));
    // We use a uint32_t instead of a uint8_t because operator<<() below will
    // ignore std::hex for uint8_t and treat it as a char instead, so the output
    // will look like garbage.
    const std::uint32_t val =
        std::accumulate(i, end, 0, std::bit_xor<std::uint8_t>());

    // setw() resets after "<< val", and so must be done here instead of above.
    output << std::setw(2) << val;

    i = end;
  }
  return output.str();
}


}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter length string: ";
  std::string input;
  std::getline(std::cin, input);

  std::vector<std::uint8_t> sparse_hash(SparseHash(Tokenize(input)));

  if (kPart1)
    std::cout << "Product: " << sparse_hash[0] * sparse_hash[1] << std::endl;
  else
    std::cout << "Knot hash: " << KnotHash(sparse_hash) << std::endl;

  return 0;
}