// Advent of Code 2017 day 17 solution
// Peter Kasting, Dec. 17, 2017

#include <cstddef>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

namespace {

constexpr bool kPart1 = false;  // Use true for part 1, false for part 2.

// Compute the value for part 1, which is the buffer element just after the last
// inserted value (2017).
std::size_t Part1Value(std::size_t steps) {
  // Build the buffer.  We use both an iterator and a position number, since we
  // need the former to insert/read out values and the latter to simulate the
  // buffer being circular.  (This also lets us reduce the cost of incrementing
  // the iterator in the wraparound case, compared to using a true circular
  // buffer.)  We could just track one of these two and compute the other, but
  // that computation is O(n) reads in the buffer size, which we don't want to
  // do at every step.  We could use a random-access type like vector to make
  // this computation O(1), but then the insertions would be O(n) reads and
  // writes in the buffer size, which is even worse.
  std::list<std::size_t> buffer = {0, 1};
  std::list<std::size_t>::iterator i = buffer.begin();
  constexpr size_t kValues = 2017;
  for (std::size_t pos = 0; buffer.size() <= kValues;
       i = buffer.insert(i, buffer.size())) {
    // |new_pos| is actually the "one past" position, i.e. the position to
    // insert before, rather than to insert after.
    const std::size_t new_pos = (pos + steps + 1) % buffer.size();
    if (new_pos < pos)
      i = std::next(buffer.begin(), new_pos);
    else
      i = std::next(i, new_pos - pos);
    pos = new_pos;
  }

  // |i| now points at the last inserted element; return the next element
  // (circularly).
  i = std::next(i, 1);
  return (i == buffer.end()) ? buffer.front() : *i;
}

// Compute the value for part 2, which is the buffer element just after the
// first element in the buffer (0).
std::size_t Part2Value(std::size_t steps) {
  // We don't actually need to build the whole buffer; since 0 never moves from
  // the front of the buffer, all we need to do is simulate the buffer size
  // increasing and see when we would have inserted a value right after 0.
  constexpr std::size_t kValues = 50000000;
  std::size_t value_after_zero = 1;
  for (std::size_t pos = 1, buf_size = 2; buf_size <= kValues; ++buf_size) {
    // |pos| is actually the "one past" position, i.e. the position to
    // insert before, rather than to insert after.  It wouldn't be correct to
    // move the "+ 1" inside the modulus; in the case where the result would be
    // zero, this wraps in this round versus the next one.  Because |buf_size|
    // is incremented between rounds, this means the wraparound is done with a
    // one-too-small value of |buf_size|, and thus |pos| at the end of the next
    // round will be one value too large.
    pos = ((pos + steps) % buf_size) + 1;
    if (pos == 1)
      value_after_zero = buf_size;
  }
  return value_after_zero;
}

}  // namespace

int main(int argc, char* argv[]) {
  std::cout << "Enter steps: ";
  std::size_t steps;
  std::cin >> steps;

  std::cout << "Value: " << (kPart1 ? Part1Value(steps) : Part2Value(steps))
            << std::endl;

  return 0;
}