// Advent of Code 2017 day 12 solution
// Peter Kasting, Dec. 11, 2017

#include <cstddef>
#include <iostream>
#include <iterator>
#include <queue>
#include <regex>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Converts the input, a series of lines giving the connections between
// programs, into a vector of the connected programs for each program.  Lines
// are assumed to be in ascending order.
std::vector<std::vector<std::size_t>> ProcessInput() {
  std::string input;
  std::vector<std::vector<std::size_t>> connections;
  const std::regex regex("\\d+");
  for (std::size_t program = 0; std::getline(std::cin, input); ++program) {
    connections.resize(program + 1);

    // Match numbers within the line, throwing away everything else as
    // delimiters.
    std::sregex_iterator i(input.begin(), input.end(), regex);
    // Skip the first number, which should be |program|.
    for (++i; i != std::sregex_iterator(); ++i) {
      std::size_t connected = static_cast<std::size_t>(std::stoi(i->str()));
      connections[program].push_back(connected);
    }
  }
  return connections;
}

// Returns the number of connected programs in the group beginning with
// |first_elem|.  |connections| gives the connections between each program.
// |found_group| is updated to flag all programs found to be part of this group.
std::size_t ProcessGroup(
    std::size_t first_elem,
    const std::vector<std::vector<std::size_t>>& connections,
    std::vector<bool>* found_group) {
  std::size_t group_size = 0;
  std::queue<std::size_t> processing;  // All the connected programs whose
                                       // connections we have yet to trace.
  for (processing.push(first_elem); !processing.empty(); processing.pop()) {
    const std::size_t program = processing.front();
    // Every connected program not already part of this group is added to the
    // group, and all its connections appended to the processing queue.
    for (std::size_t candidate : connections[program]) {
      if (!(*found_group)[candidate]) {
        (*found_group)[candidate] = true;
        processing.push(candidate);
        ++group_size;
      }
    }
  }
  return group_size;
}

// Returns the next ungrouped element after |elem|, that is, the next element n
// for which found_group[n] is false.  If no such element exists, returns
// found_group.size().
std::size_t GetUngroupedElemAfter(std::size_t elem,
                                  const std::vector<bool>& found_group) {
  do {
    ++elem;
  } while ((elem < found_group.size()) && found_group[elem]);
  return elem;
}

// Returns the number of groups partitioning |connections|.
std::size_t CountGroups(
    const std::vector<std::vector<std::size_t>>& connections,
    std::vector<bool>* found_group) {
  std::size_t groups = 0;
  for (std::size_t first_elem = 0; first_elem < connections.size();
       first_elem = GetUngroupedElemAfter(first_elem, *found_group)) {
    ProcessGroup(first_elem, connections, found_group);
    ++groups;
  }
  return groups;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter program data; terminate with ctrl-z alone on a line."
            << std::endl;

  std::vector<std::vector<std::size_t>> connections = ProcessInput();
  std::vector<bool> found_group(connections.size(), false);
  // In part 1, we want the size of the first group (the group containing
  // element 0); in part 2, the number of groups.
  if (kPart1) {
    std::cout << "Connected programs: "
              << ProcessGroup(0, connections, &found_group) << std::endl;
  } else {
    std::cout << "Groups: " << CountGroups(connections, &found_group)
              << std::endl;
  }

  return 0;
}