// Advent of Code 2017 day 11 solution
// Peter Kasting, Dec. 10, 2017

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// A coordinate in a hex grid.  Columns of hexes are 1 horizontal unit apart
// while within a column, each hex center is two vertical units apart.  This
// allows adjacent columns to be vertically offset from each other.  The origin
// is in the center of an infinite grid, with +x to the right and +y upwards.
class Coord {
public:
  Coord() = default;
  ~Coord() = default;

  // Moves the coordinate one hex in |direction|, which is one of
  // {"nw", "n", "ne", "se", "s", "sw"}.
  void Move(const std::string& direction) {
    // Either we're travelling vertically within the current column, or
    // diagonally to a hex in an adjacent column.
    const bool diagonal = direction.length() > 1;
    if (diagonal)
      x += (direction[1] == 'e') ? 1 : -1;
    const int delta_y = diagonal ? 1 : 2;
    y += (direction[0] == 'n') ? delta_y : -delta_y;
  }

  // Returns how many steps (hexes) away from the origin the coordinate
  // currently is.
  int StepsFromOrigin() const {
    // Each column we traverse horizontally takes us diagonally, and thus can
    // account for a delta of up to 1 in the y coordinate.  (We can always
    // alternate "n"- and "s"-diagonal moves to avoid changing y, if we don't
    // need to move that far vertically.)
    const int diagonal_steps = std::abs(x);
    const int remaining_y_distance = std::max(std::abs(y) - diagonal_steps, 0);

    // Once we've reached the desired column, the desired y coordinate is a
    // multiple of 2 away, since adjacent hexes within a column are two units
    // apart.
    return diagonal_steps + (remaining_y_distance / 2);
  }

private:
  int x = 0, y = 0;
};

// Returns the distance from the origin travelled by the path |input|.  In part
// 1, this is the net distance; in part 2, the furthest distance away.
int GetDistance(const std::string& input) {
  std::istringstream stringstream(input);
  Coord coord;
  int max_steps = 0;
  for (std::string direction; std::getline(stringstream, direction, ','); ) {
    coord.Move(direction);
    max_steps = std::max(max_steps, coord.StepsFromOrigin());
  }
  return kPart1 ? coord.StepsFromOrigin() : max_steps;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter path: ";
  std::string input;
  std::getline(std::cin, input);

  std::cout << "Steps away: " << GetDistance(input) << std::endl;

  return 0;
}