// Advent of Code 2017 day 2 solution
// Peter Kasting, Dec. 4, 2017

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
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

// Computes the checksum portion for the given |row|.  In part 1, this is the
// difference between the largest and smallest elements; in part 2, the quotient
// of the two elements that evenly divide.
int ComputeRowChecksum(const std::vector<int>& row) {
  if (kPart1) {
    // Return the difference between the smallest and largest elements.
    const auto minmax_iters = std::minmax_element(row.cbegin(), row.cend());
    return *minmax_iters.second - *minmax_iters.first;
  }

  // Find the pair of elements where the smaller evenly divides the larger.
  //
  // This uses brute-force comparison of all pairs, which is n^2.  We could sort
  // first to avoid trying to divide smaller elements by larger ones, but given
  // that this only reduces the cost from (n^2 - n) to ((n^2 + n) / 2), and
  // sorting costs (n lg n), it's not a huge win.
  for (auto i = row.cbegin(); i != row.cend(); ++i) {
    for (auto j = row.cbegin(); j != row.cend(); ++j) {
      if ((i != j) && (*i % *j == 0))
        return *i / *j;
    }
  }
  return 0;
}

}  // namespace

int main(int argc, char* argv[]) {
  // Read input line-at-a-time and compute running checksum.
  //
  // We could store the row checksums in a vector and use std::accumulate(), but
  // that would require more memory.
  std::cout << "Enter spreadsheet rows; terminate with ctrl-z alone on a line."
            << std::endl;
  std::string row;
  int checksum = 0;
  while (std::getline(std::cin, row))
    checksum += ComputeRowChecksum(Tokenize(row));

  std::cout << "Checksum is: " << checksum << std::endl;
  return 0;
}
