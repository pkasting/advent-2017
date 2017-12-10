// Advent of Code 2017 day 7 solution
// Peter Kasting, Dec. 8, 2017

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/* Some test inputs to exercise edge cases:
 * Nodes with only one child:

a (10) -> b, c, d
b (72)
c (7) -> e
d (72)
e (50) -> f
f (3) -> g, h
g (10)
h (10)

Desired output: 42

 * Nodes with a mismatched pair:

a (10) -> b, c
b (20) -> d, e
c (34)
d (5) -> f, g
e (10) -> h, i
f (1)
g (1)
h (1)
i (1)

Desired output: 5

a (10) -> b, c
b (20) -> d, e
c (44)
d (5) -> f, g
e (10) -> h, i
f (1)
g (1)
h (1)
i (1)

Desired output: 10

a (10) -> b, c
b (44)
c (20) -> d, e
d (5) -> f, g
e (10) -> h, i
f (1)
g (1)
h (1)
i (1)

Desired output: 10

 */

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

class Node {
 public:
  Node(int weight, std::vector<std::string> child_names);

  // Uses the provided |map| to depth-first convert this node and its children
  // into a subtree.  This also computes much of the weight and balance
  // information needed by ReplacementWeight(), since it's easy to do while
  // already traversing the tree.
  void MakeTree(std::unordered_map<std::string, Node>* map);

  // Given that this tree contains a single node with an incorrect value,
  // returns the correct value for that node.  MakeTree() must be called before
  // calling this function.
  int ReplacementWeight() const;

 private:
  // Implementation of ReplacementWeight() for a specific subtree, which is
  // expected to weigh |expected_weight|.
  int ReplacementWeight(int expected_weight) const;

  const int weight_;  // Weight not including subtrees.
  const std::vector<std::string> child_names_;

  // Values computed during MakeTree(), so ReplacementWeight() doesn't have to
  // recompute them at every level:
  std::vector<Node*> subtrees_;
  int total_weight_;          // Weight including subtrees.
  Node* unbalanced_subtree_;  // May be null if subtrees are balanced.
  bool mismatched_pair_;      // True when we have exactly two subtrees, their
                              // weights disagree, and they have no unbalanced
                              // subtrees of their own.
};

Node::Node(int weight, std::vector<std::string> child_names)
    : weight_(weight),
      child_names_(std::move(child_names)),
      total_weight_(weight),
      unbalanced_subtree_(nullptr) {}

void Node::MakeTree(std::unordered_map<std::string, Node>* map) {
  // If all subtrees are balanced, their weights will all agree, so each subtree
  // after the first will have the same weight as the first.  These variables
  // let us detect if that's not the case.
  int first_subtree_weight;
  bool second_subtree_differs = false;

  for (auto i = child_names_.begin(); i != child_names_.end(); ++i) {
    // Hook up each subtree.
    Node* const subtree = &map->find(*i)->second;
    subtrees_.push_back(subtree);
    subtree->MakeTree(map);

    total_weight_ += subtree->total_weight_;

    // Look for signs any subtrees are unbalanced.
    if (!unbalanced_subtree_) {
      if (subtree->unbalanced_subtree_ || subtree->mismatched_pair_) {
        // |subtree| contains a problem, so point at it.
        unbalanced_subtree_ = subtree;
      } else if (i == child_names_.begin()) {
        first_subtree_weight = subtree->total_weight_;
      } else if (second_subtree_differs) {
        // The first two subtrees' weights disagree; use this subtree's weight
        // to determine who's correct.
        const bool first_ok = subtree->total_weight_ == first_subtree_weight;
        unbalanced_subtree_ = subtrees_[first_ok ? 1 : 0];
      } else if (subtree->total_weight_ != first_subtree_weight) {
        // This subtree's weight disagrees with the first.  If it's the second
        // subtree we don't yet know which is wrong, but if not, the first two
        // subtrees must have agreed, so this one must be wrong.
        if (i == std::next(child_names_.begin()))
          second_subtree_differs = true;
        else
          unbalanced_subtree_ = subtree;
      }
    }
  }

  // Not enough information in this case to determine which subtree is wrong;
  // we'll have to handle this in ReplacementWeight().
  mismatched_pair_ = !unbalanced_subtree_ && second_subtree_differs;
}

int Node::ReplacementWeight() const {
  // The only time the value here matters is in the error case when the tree is
  // fully-balanced.  Passing |total_weight_| will cause us to report that as
  // the replacement weight, i.e. replace the root weight with itself.
  return ReplacementWeight(total_weight_);
}

int Node::ReplacementWeight(int expected_weight) const {
  if (unbalanced_subtree_) {
    // We know which subtree is problematic.  Pass it the weight of a different
    // subtree as a known-good reference.
    const std::size_t ok = (unbalanced_subtree_ == subtrees_.front()) ? 1 : 0;
    return unbalanced_subtree_->ReplacementWeight(subtrees_[ok]->total_weight_);
  }

  // When we have a single, balanced subtree, and we're trying to add weight, we
  // can replace either our own weight or the one in the subtree; we choose to
  // replace our own weight in this case, which is handled below.  If we're
  // trying to cut weight, then whether we can replace our own weight depends on
  // whether it's large enough to absorb the proposed cut.  If not, we have to
  // pass the request on to the subtree.
  if ((subtrees_.size() == 1) && ((total_weight_ - expected_weight) > weight_))
    return subtrees_.front()->ReplacementWeight(expected_weight - weight_);

  if (mismatched_pair_) {
    // Two mismatched subtrees.  One is larger than the other, so either we need
    // to increase weight (and the smaller should be made to match the larger)
    // or decrease weight (and the larger should be made to match the smaller).
    const Node* first = subtrees_[0], * second = subtrees_[1];
    if ((expected_weight > total_weight_) !=
        (first->total_weight_ < second->total_weight_))
      std::swap(first, second);
    return first->ReplacementWeight(second->total_weight_);
  }

  // Nothing is wrong with any of our subtrees, so we replace our own weight.
  return weight_ + expected_weight - total_weight_;
}

// Parses |input| into a program name, weight, and (possibly empty) set of
// subprograms.
std::pair<std::string, Node> ParseProgramInfo(
    const std::string& input,
    std::unordered_set<std::string>* subprograms) {
  // Find initial program name.
  const std::regex name_regex("[a-z]+");
  std::sregex_iterator i(input.begin(), input.end(), name_regex);
  const std::string program_name = i->str();

  // Any other program names are subprograms.
  std::vector<std::string> subprogram_names;
  for (++i; i != std::sregex_iterator(); ++i) {
    subprogram_names.push_back(i->str());
    subprograms->insert(i->str());
  }

  // Any number mentioned is the weight.
  const std::regex weight_regex("\\d+");
  std::smatch weight_match;
  std::regex_search(input, weight_match, weight_regex);

  return {program_name, Node(std::stoi(weight_match.str()), subprogram_names)};
}

// Returns the name of the root program, given the sets of all programs and all
// programs known to be subprograms.
std::string RootProgram(const std::unordered_map<std::string, Node>& programs,
                        const std::unordered_set<std::string>& subprograms) {
  // The root program is simply whichever program is not mentioned in the set of
  // all subprograms.
  const auto NotSubprogram =
      [&subprograms](const auto& i) { return !subprograms.count(i.first); };
  return std::find_if(programs.cbegin(), programs.cend(), NotSubprogram)->first;
}

}  // namespace

int main(int argc, char* argv[]) {
  // Read input line-at-a-time and parse into program nodes.
  std::cout << "Enter program data; terminate with ctrl-z alone on a line."
            << std::endl;
  std::string program_info;
  std::unordered_map<std::string, Node> programs;
  std::unordered_set<std::string> subprograms;
  while (std::getline(std::cin, program_info))
    programs.insert(ParseProgramInfo(program_info, &subprograms));

  // Compute root program.
  const std::string root_name = RootProgram(programs, subprograms);
  if (kPart1) {
    // This is all we need for part 1.
    std::cout << "Bottom program: " << root_name << std::endl;
  } else {
    // Convert the nodes to a true tree structure.  We could avoid this by doing
    // map lookups each time a node needed to access a child, but since we have
    // to visit all the nodes at least once anyway for that, we might as well
    // use a tree.  This also makes the code in the Node class cleaner, since
    // use of the map is limited to Node::MakeTree().
    Node* root = &programs.find(root_name)->second;
    root->MakeTree(&programs);
    std::cout << "Replacement weight: " << root->ReplacementWeight()
              << std::endl;
  }
  return 0;
}
