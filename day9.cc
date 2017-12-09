// Advent of Code 2017 day 9 solution
// Peter Kasting, Dec. 8, 2017

#include <iostream>
#include <string>
#include <tuple>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

std::tuple<int, int> ComputeScores(const std::string& stream) {
  // Count the score of the groups in the stream and the number of total garbage
  // characters.  We need the former for part 1 and the latter for part 2.
  int total_score = 0;
  int group_score = 0;
  int garbage_chars = 0;
  bool in_garbage = false;
  for (auto i = stream.begin(); i != stream.end(); ++i) {
    const char c = *i;
    // Which checks come before/after the |in_garbage| check is important for
    // precedence reasons.
    if (c == '!')
      ++i;
    else if (c == '>')
      in_garbage = false;
    else if (in_garbage)
      ++garbage_chars;
    else if (c == '{')
      ++group_score;
    else if (c == '}')
      total_score += group_score--;
    else if (c == '<')
      in_garbage = true;
  }
  return {total_score, garbage_chars};
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter stream: ";

  std::string stream;
  std::cin >> stream;

  // Count the score of the groups in the stream and the number of total garbage
  // characters.  We need the former for part 1 and the latter for part 2.
  int total_score = 0;
  int group_score = 0;
  int garbage_chars = 0;
  bool in_garbage = false;
  for (auto i = stream.begin(); i != stream.end(); ++i) {
    const char c = *i;
    // Which checks come before/after the |in_garbage| check is important for
    // precedence reasons.
    if (c == '!')
      ++i;
    else if (c == '>')
      in_garbage = false;
    else if (in_garbage)
      ++garbage_chars;
    else if (c == '{')
      ++group_score;
    else if (c == '}')
      total_score += group_score--;
    else if (c == '<')
      in_garbage = true;
  }

  if (kPart1)
    std::cout << "Score: " << total_score << std::endl;
  else
    std::cout << "Garbage characters: " << garbage_chars << std::endl;

  return 0;
}