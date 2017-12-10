// Advent of Code 2017 day 3 solution
// Peter Kasting, Dec. 4, 2017
//
// It took a while to figure out a constant-time solution for part 1...

#include <array>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// A dynamically-sized two-dimensional block of simulated memory.  We could save
// quite a bit of code by using a std::map<std::pair<int, int>, int> to store
// values indexed by spiral coords, but it would be slower and require more
// memory.
using Memory = std::vector<std::vector<int>>;

// Returns various useful bits of positioning info within the spiral, given a
// (zero-based) address.  This is based on treating the spiral as a series of
// concentric square rings, numbered from 0 (the innermost ring containing only
// one square) up.
std::tuple<int, int, int, int> GetPositioningInfo(int address) {
  // Compute which ring this address is on.  Each added ring forms a larger
  // square, so the ring number is computable by taking the square root of the
  // address.  The squares have sides of odd length, so to compute the ring
  // number we need to divide this square root by two and round appropriately.
  const int ring = static_cast<int>((std::sqrt(address) + 1) / 2);

  // Compute the length of each side of this ring, including only one corner.
  // (By including only one corner, we can exactly cover the ring using four
  // sides, without overlapping).
  const int side_length = ring * 2;

  // Compute the position of this address around the ring, starting from the
  // position above the bottom right corner of the ring (the first address in
  // this ring).  This is simply the global address minus the number of squares
  // enclosed by this ring.
  const int enclosed_squares = (side_length - 1) * (side_length - 1);
  const int pos = address - enclosed_squares;

  // Convert the position around the ring to an offset from the center of the
  // side.
  const int pos_along_side = pos % side_length;
  const int offset = pos_along_side + 1 - ring;  // Note ring = side_length / 2.
                                                 // +1 is because side has no
                                                 // leading corner.

  return {ring, offset, pos, side_length};
}

// Returns the Manhattan distance from the center of the spiral to |address|.
int ManhattanDistance(int address) {
  // Input address is one-indexed.  Convert to zero-indexed; what
  // self-respecting system would use one-indexed memory addressing?
  --address;

  // Calculate Manhattan distance.  The ring number gives the distance from the
  // center of the spiral to the center of the nearest side of the ring, and the
  // offset gives the distance along the side from the center.
  auto pos_info = GetPositioningInfo(address);
  return std::get<0>(pos_info) + std::abs(std::get<1>(pos_info));
}

// Converts a coordinate in the spiral system (with 0 in the center) to an index
// into the simulated memory array.
std::size_t ConvertCoord(int coord) {
  // In order to store things by index in a vector, indexes must be positive.
  // To achieve this, we interleave positive- and negative-index coordinates:
  // {0, -1, 1, -2, 2, -3, 3, ...}.
  coord *= 2;
  return static_cast<std::size_t>((coord >= 0) ? coord : (-coord - 1));
}

// Returns the value at spiral coord (x, y) in |memory|.  If (x, y) has not been
// written yet, returns 0.
int Read(const Memory& memory, int x, int y) {
  // Get the matching memory column for |x|.
  const std::size_t column_index = ConvertCoord(x);
  if (memory.size() <= column_index)
    return 0;
  const auto column = memory[column_index];

  // Return the appropriate row from that column.
  const std::size_t row_index = ConvertCoord(y);
  return (column.size() > row_index) ? column[row_index] : 0;
}

// Writes |value| to spiral coord (x, y) in |memory|.
void Write(Memory* memory, int x, int y, int value) {
  // Get the matching memory column for |x|.
  const std::size_t column_index = ConvertCoord(x);
  if (memory->size() <= column_index)
    memory->resize(column_index + 1);  // Column not yet written.
  auto& column = (*memory)[column_index];

  // And the matching row within that column.
  const std::size_t row_index = ConvertCoord(y);
  if (column.size() <= row_index)
    column.resize(row_index + 1);  // Row not yet written.

  // Write the value.  Note that any other slots created by the resize() calls
  // above will have been value-initialized, which, for ints, means "set to 0".
  // This is what we want, so that unwritten values will read as 0 whether
  // they've been allocated (here) or not (and thus handled by bounds-checks in
  // Read()).
  column[row_index] = value;
}

// Fills memory with appropriate values until one is larger than the input, and
// returns that value.  Each value is the sum of all neighboring
// previously-written values.
int FirstLargerValue(int input) {
  // Write 1 to the initial slot in memory (at (0, 0)).
  Memory memory;
  Write(&memory, 0, 0, 1);

  // Write the sum of all surrounding values to each successive address.
  int value = 0;
  for (int address = 1; value <= input; ++address) {
    // Convert |address| to spiral coord (|x|, |y|).  These are basically the
    // ring and offset values for the address, but swapped/sign-flipped based on
    // which side of the ring the address is on.
    //
    // We could instead compute whether to increment or decrement |x| or |y| by
    // one at each step.  But since GetPositioningInfo() is already O(1), this
    // would be more code for very little efficiency gain.
    int ring, offset, pos, side_length;
    std::tie(ring, offset, pos, side_length) = GetPositioningInfo(address);
    const int side = pos / side_length;
    // Somehow it seems like I should be able to omit std::array here and use a
    // built-in array, but I couldn't find a syntax that compiled...
    const int x = std::array<int, 4>{ring, -offset, -ring, offset}[side];
    const int y = std::array<int, 4>{offset, ring, -offset, -ring}[side];

    // Sum the 8 surrounding slots and write back to memory.  This actually adds
    // the value in the current slot as well, but its value is always 0, so that
    // doesn't matter.
    value = 0;
    for (int x_offset : {-1, 0, 1}) {
      for (int y_offset : {-1, 0, 1})
        value += Read(memory, x + x_offset, y + y_offset);
    }
    Write(&memory, x, y, value);
  }
  return value;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter value: ";
  int input;
  std::cin >> input;

  if (kPart1)
    std::cout << "Steps required: " << ManhattanDistance(input) << std::endl;
  else
    std::cout << "First larger value: " << FirstLargerValue(input) << std::endl;
  return 0;
}
