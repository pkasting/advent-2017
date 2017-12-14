// Advent of Code 2017 day 14 solution
// Peter Kasting, Dec. 14, 2017

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Converts |input| to a series of lengths to use to compute the sparse hash.
// This series consists of the byte values of each input character, plus a
// suffix.
std::vector<std::size_t> Tokenize(const std::string& input) {
  // Treat each character as a length.
  std::vector<std::size_t> lengths;
  for (char c : input)
    lengths.push_back(c);

  // Add hardcoded suffix.
  constexpr std::size_t kSuffix[5] = {17, 31, 73, 47, 23};
  lengths.insert(lengths.end(), std::begin(kSuffix), std::end(kSuffix));

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

  // Permute for 64 rounds, preserving the position and skip length between
  // rounds.
  constexpr int kRounds = 64;
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
std::vector<bool> KnotHash(const std::vector<std::uint8_t>& sparse_hash) {
  std::vector<bool> hash;
  for (auto i = sparse_hash.cbegin(); i != sparse_hash.cend(); ) {
    // XOR the next (up to) 16 numbers together.
    constexpr std::size_t kValuesToXor = 16;
    const size_t remaining_size =
        static_cast<std::size_t>(std::distance(i, sparse_hash.end()));
    const auto end = std::next(i, std::min(kValuesToXor, remaining_size));
    const std::uint8_t val =
        std::accumulate(i, end, 0, std::bit_xor<std::uint8_t>());

    // Concatenate the bits of |val| to |hash|.
    for (std::uint8_t mask = 0x80; mask; mask >>= 1)
      hash.push_back(!!(val & mask));

    i = end;
  }
  return hash;
}

// Constructs a 128-row grid of on/off squares based on knot hashes computed
// from successively-modified versions of |input|.
std::vector<std::vector<bool>> ConstructGrid(const std::string& input) {
  std::vector<std::vector<bool>> grid;
  constexpr std::size_t kGridRows = 128;
  for (std::size_t i = 0; i < kGridRows; ++i) {
    grid.push_back(
        KnotHash(SparseHash(Tokenize(input + "-" + std::to_string(i)))));
  }
  return grid;
}

// Returns the total number of "used squares" (true values) in |grid|.
std::size_t CountSquares(const std::vector<std::vector<bool>>& grid) {
  const auto SquaresInRow =
      [](std::size_t count, const auto& row) {
    return count + std::count(row.cbegin(), row.cend(), true);
  };
  return std::accumulate(grid.cbegin(), grid.cend(), 0, SquaresInRow);
}

// Returns the neighboring grid coordinates to |coord|.  This is normally the
// four adjacent squares, except when |coord| is on a grid edge.
std::vector<std::pair<std::size_t, std::size_t>> Neighbors(
    const std::pair<std::size_t, std::size_t>& coord,
    const std::vector<std::vector<bool>>& grid) {
  std::vector<std::pair<std::size_t, std::size_t>> neighbors;
  if (coord.first > 0)
    neighbors.push_back({coord.first - 1, coord.second});
  if (coord.second > 0)
    neighbors.push_back({coord.first, coord.second - 1});
  if (coord.second < (grid[coord.first].size() - 1))
    neighbors.push_back({coord.first, coord.second + 1});
  if (coord.first < (grid.size() - 1))
    neighbors.push_back({coord.first + 1, coord.second});
  return neighbors;
}

// Marks as visited (by erasing) all squares of a contiguous region in |grid|
// containing (row, col).
void ProcessRegion(std::size_t row,
                   std::size_t col,
                   std::vector<std::vector<bool>>* grid) {
  std::queue<std::pair<std::size_t, std::size_t>> processing;

  // Marks (row, col) as visited, and if it's used, adds it to the processing
  // queue.
  const auto Visit =
      [grid, &processing](std::size_t row, std::size_t col) {
    if ((*grid)[row][col]) {
      processing.push({row, col});
      (*grid)[row][col] = false;
    }
  };

  // Check the first square of the region.
  Visit(row, col);
  while (!processing.empty()) {
    // Found a valid region.  Breadth-first visit all contiguous squares.
    for (const auto& neighbor : Neighbors(processing.front(), *grid))
      Visit(neighbor.first, neighbor.second);
    processing.pop();
  }
 }

// Returns the number of distinct contiguous regions of used squares in |grid|.
std::size_t CountRegions(std::vector<std::vector<bool>> grid) {
  std::size_t regions = 0;
  for (std::size_t row = 0; row < grid.size(); ++row) {
    for (std::size_t col = 0; col < grid[row].size(); ++col) {
      if (grid[row][col]) {
        ++regions;
        ProcessRegion(row, col, &grid);
      }
    }
  }
  return regions;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter input string: ";

  std::string input;
  std::getline(std::cin, input);
  const std::vector<std::vector<bool>> grid = ConstructGrid(input);

  if (kPart1)
    std::cout << "Squares used: " << CountSquares(grid) << std::endl;
  else
    std::cout << "Regions: " << CountRegions(grid) << std::endl;

  return 0;
}