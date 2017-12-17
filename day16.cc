// Advent of Code 2017 day 16 solution
// Peter Kasting, Dec. 17, 2017

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.
constexpr std::size_t kNumPrograms = 16;  // How many programs are dancing.

// Returns the string "abc...", with kNumPrograms total characters.
std::string GeneratePrograms() {
  std::string programs(kNumPrograms, 'a');
  std::iota(programs.begin() + 1, programs.end(), 'b');
  return programs;
}

// Given an encoded |move|, updates the |position_transform| or |name_transform|
// strings to reflect applying that move.
void ParseMove(const std::string& move,
               std::string* position_transform,
               std::string* name_transform) {
  switch (move[0]) {
    case 'p': {
      // "Partner": Exchange the programs with names before/after '/'.  This
      // changes the names of programs irrespective of their positions, and so
      // updates |name_transform|.
      std::swap((*name_transform)[name_transform->find(move[1])],
                (*name_transform)[name_transform->find(move[3])]);
      break;
    }
    case 's':
      // "Spin": rotate n programs from the back to the front.  This moves
      // programs irrespective of their names, and so updates
      //|position_transform|.
      std::rotate(position_transform->begin(),
                  position_transform->end() - std::stoi(move.substr(1)),
                  position_transform->end());
      break;
    case 'x': {
      // "Exchange": Exchange the programs with positions before/after '/'.
      // This moves programs irrespective of their names, and so updates
      // |name_transform|.
      const std::size_t slash = move.find('/', 2);
      std::swap((*position_transform)[std::stoi(move.substr(1, slash - 1))],
                (*position_transform)[std::stoi(move.substr(slash + 1))]);
      break;
    }
  }
}

// A series of functions to convert between positions (0, 1, ...) and names
// ('a', 'b', ...).  To be called by other templated functions whose template
// type is the desired return type here, hence the function name "to T".
template<typename T> inline T ToT(std::size_t pos);

template<>
inline std::size_t ToT(std::size_t pos) {
  return pos;
}

template<>
inline char ToT(std::size_t pos) {
  return static_cast<char>(pos + 'a');
}

template<typename T> inline T ToT(char name);

template<>
inline std::size_t ToT(char name) {
  return static_cast<std::size_t>(name - 'a');
}

template<>
inline char ToT(char name) {
  return name;
}

// Returns the next unvisited position after |start|, given |visited| tracks
// whether each position has been visited.
std::size_t FindNextUnvisited(const std::vector<bool>& visited,
                              std::size_t start) {
  return std::distance(visited.cbegin(),
                       std::find(std::next(visited.cbegin(), start + 1),
                                 visited.cend(), false));
}

// Returns a map containing the cycle of transforms in |transform| beginning at
// |start|, and updates |start| to the next unvisited program.  |visited| must
// be preserved across calls since a previous cycle may have visited positions
// after |start|.
template<typename T>
std::map<T, T> FindCycle(const std::string& transform,
                         std::size_t* start,
                         std::vector<bool>* visited) {
  // First add a mapping from the starting position to whatever we find there.
  // Note that doesn't bother to set (*visited)[*start], but that doesn't matter
  // since we'll never read that value.
  std::map<T, T> cycle;
  const T start_as_t = ToT<T>(*start);
  const T first = ToT<T>(transform[*start]);
  cycle[start_as_t] = first;

  // Then loop from that next position back to the starting position again.
  for (T next = first; next != start_as_t; ) {
    // Mark the current position as visited.
    const std::size_t pos = ToT<std::size_t>(next);
    (*visited)[pos] = true;

    // Add a mapping from the current position to the next one.
    const T old_next = next;
    next = ToT<T>(transform[pos]);
    cycle[old_next] = next;
  }

  // Update |start|.
  *start = FindNextUnvisited(*visited, *start);

  return cycle;
}

// Returns a vector of the cycles that partition |transform|.
template<typename T>
std::vector<std::map<T, T>> FindCycles(const std::string& transform) {
  std::vector<std::map<T, T>> cycles;
  std::vector<bool> visited(kNumPrograms, false);
  for (std::size_t start_pos = 0; start_pos != kNumPrograms; )
    cycles.push_back(FindCycle<T>(transform, &start_pos, &visited));
  return cycles;
}

// Applies transform cycle |cycle| to |programs|.
template<typename T>
void ApplyCycle(const std::map<T, T>& cycle, std::string* programs);

// Implementation of ApplyCycle() for a position transform.
template<>
void ApplyCycle<std::size_t>(const std::map<std::size_t, std::size_t>& cycle,
                             std::string* programs) {
  // For every element in the transform, write the value at the old position to
  // the transformed position.
  std::string output = *programs;
  for (const auto& i : cycle)
    output[i.first] = (*programs)[i.second];
  programs->swap(output);
}

// Implementation of ApplyCycle() for a name transform.
template<>
void ApplyCycle<char>(const std::map<char, char>& cycle,
                      std::string* programs) {
  // For every program that's named in the transform, rename to the transformed
  // name.
  std::string output = *programs;
  for (char& c : output) {
    const auto i = cycle.find(c);
    if (i != cycle.end())
      c = i->second;
  }
  programs->swap(output);
}

// Applies all transform |cycles| to |programs| |dances| times.
template<typename T>
void ApplyCycles(const std::vector<std::map<T, T>>& cycles,
                 std::size_t dances,
                 std::string* programs) {
  for (const auto& cycle : cycles) {
    // Since |cycle| is a cycle, it is the identity transform when applied
    // cycle.size() times; so we don't need to apply it |dances| times, but only
    // that value modulo the cycle size.
    for (std::size_t i = 0; i < (dances % cycle.size()); ++i)
      ApplyCycle<T>(cycle, programs);
  }
}

// Returns the transformed program string that results from applying
// |position_transform| and |name_transform| a number of times.  In part 1, this
// number is 1; in part 2, it's 1,000,000,000.
std::string TransformedPrograms(const std::string& position_transform,
                                const std::string& name_transform) {
  constexpr std::size_t kNumDances = kPart1 ? 1 : 1000000000;
  std::string programs = GeneratePrograms();
  ApplyCycles<std::size_t>(FindCycles<std::size_t>(position_transform),
                           kNumDances, &programs);
  ApplyCycles<char>(FindCycles<char>(name_transform), kNumDances, &programs);
  return programs;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter moves, separated by commas: ";

  // Compute the transforms from the input.
  std::string move;
  std::string position_transform = GeneratePrograms();
  std::string name_transform = GeneratePrograms();
  while (std::getline(std::cin, move, ','))
    ParseMove(move, &position_transform, &name_transform);

  // Transform the program string.
  std::cout << TransformedPrograms(position_transform, name_transform)
            << std::endl;

  return 0;
}