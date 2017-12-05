// Advent of Code 2017 day 4 solution
// Peter Kasting, Dec. 4, 2017

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_set>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Returns true if |passphrase| is valid.  In part 1, a valid passphrase
// contains no duplicate words.  In part 2, it contains no words that are
// anagrams of each other.
bool PassphraseValid(const std::string& passphrase) {
  // Convert the passphrase to a set of words, checking each against previous
  // words in the passphrase.
  std::istringstream stringstream(passphrase);
  std::unordered_set<std::string> words;
  const auto ProcessWord = [](std::string word) {
    if (!kPart1) {
      // Map all anagrams to the same word by sorting the characters.
      std::sort(word.begin(), word.end());
    }
    return word;
  };
  for (std::istream_iterator<std::string> i(stringstream), end; i != end; ++i) {
    // If the next word was already seen, mark the passphrase invalid.
    if (!words.insert(ProcessWord(*i)).second)
      return false;
  }
  return true;
}

}  // namespace

int main(int argc, char* argv[]) {
  // Read input line-at-a-time and compute running count of valid passphrases.
  //
  // We could store the passphrases in a vector and use std::count_if(), but
  // that would require more memory.
  std::cout << "Enter passphrases; terminate with ctrl-z alone on a line."
            << std::endl;
  std::string passphrase;
  int valid_passphrases = 0;
  while (std::getline(std::cin, passphrase)) {
    if (PassphraseValid(passphrase))
      ++valid_passphrases;
  }

  std::cout << "Valid passphrases: " << valid_passphrases << std::endl;
  return 0;
}
