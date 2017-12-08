// Advent of Code 2017 day 8 solution
// Peter Kasting, Dec. 7, 2017

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Breaks a string of whitespace-delimited tokens to a vector of tokens.
std::vector<std::string> Tokenize(const std::string& input) {
  return std::vector<std::string>(
             (std::istream_iterator<std::string>(std::istringstream(input))),
             std::istream_iterator<std::string>());
}

// Returns the result of "a OP b", where |op| is the string form of the binary
// operation OP.
bool Compare(int a, const std::string& op, int b) {
  if (op == "<")
    return a < b;
  if (op == "<=")
    return a <= b;
  if (op == "==")
    return a == b;
  if (op == "!=")
    return a != b;
  if (op == ">=")
    return a >= b;
  if (op == ">")
    return a > b;
  return false;  // Shouldn't reach here with well-formed input.
}

// Executes the conditional instruction in |tokens| using the register file
// |registers|.  The instruction is of the form "r1 {inc,dec} val1 if r2 OP
// val2", where r1 and r2 are registers which may or may not already be in the
// register file, val1 and val2 are ints, and OP is a binary opcode.  Returns
// the value written to r1, or 0 if the instruction is not executed.
int Execute(const std::vector<std::string> tokens,
            std::unordered_map<std::string, int>* registers) {
  if (!Compare((*registers)[tokens[4]], tokens[5], std::stoi(tokens[6])))
    return 0;

  int& reg = (*registers)[tokens[0]];
  const int increment = std::stoi(tokens[2]);
  reg += (tokens[1] == "dec") ? -increment : increment;
  return reg;
}

// Returns the maximum value currently in the register file |registers|.
int MaxValue(const std::unordered_map<std::string, int> registers) {
  return std::max_element(registers.cbegin(), registers.cend(),
                          [](const auto& a, const auto& b) {
                            return a.second < b.second;
                          })->second;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter instructions; terminate with ctrl-z alone on a line."
            << std::endl;

  // Read input line-at-a-time and execute it on the fly, keeping track of the
  // largest value written.
  std::string line;
  int max_value = 0;
  std::unordered_map<std::string, int> registers;
  while (std::getline(std::cin, line))
    max_value = std::max(max_value, Execute(Tokenize(line), &registers));

  // In part 1, print the largest value currently in the register file; in part
  // 2, print the largest value written.
  std::cout << "Max value: " << (kPart1 ? MaxValue(registers) : max_value)
            << std::endl;

  return 0;
}