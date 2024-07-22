#include "internal.hpp"

namespace CaDiCaL {

// Slightly different than 'bump_variable' since the variable is not
// enqueued at all.

inline void Internal::init_enqueue (int idx) {
  Link &l = links[idx];
  if (opts.reverse) {
    l.prev = 0;
    if (queue.first) {
      assert (!links[queue.first].prev);
      links[queue.first].prev = idx;
      btab[idx] = btab[queue.first] - 1;
    } else {
      assert (!queue.last);
      queue.last = idx;
      btab[idx] = 0;
    }
    assert (btab[idx] <= stats.bumped);
    l.next = queue.first;
    queue.first = idx;
    if (!queue.unassigned)
      update_queue_unassigned (queue.last);
  } else {
    l.next = 0;
    if (queue.last) {
      assert (!links[queue.last].next);
      links[queue.last].next = idx;
    } else {
      assert (!queue.first);
      queue.first = idx;
    }
    btab[idx] = ++stats.bumped;
    l.prev = queue.last;
    queue.last = idx;
    update_queue_unassigned (queue.last);
  }
}

// Initialize VMTF queue from current 'old_max_var + 1' to 'new_max_var'.
// This incorporates an initial variable order.  We currently simply assume
// that variables with smaller index are more important.  This is the same
// as in MiniSAT (implicitly) and also matches the 'scores' initialization.
//
void Internal::init_queue (int old_max_var, int new_max_var) {
  LOG ("initializing VMTF queue from %d to %d", old_max_var + 1,
       new_max_var);
  assert (old_max_var < new_max_var);
  //assert (!level || external_prop); // Commented due to extended resolution
  for (int idx = old_max_var; idx < new_max_var; idx++)
    init_enqueue (idx + 1);
}

// Shuffle the VMTF queue.

void Internal::shuffle_queue () {
  if (!opts.shuffle)
    return;
  if (!opts.shufflequeue)
    return;
  stats.shuffled++;
  LOG ("shuffling queue");
  vector<int> shuffle;
  if (opts.shufflerandom) {
    for (int idx = max_var; idx; idx--)
      shuffle.push_back (idx);
    Random random (opts.seed); // global seed
    random += stats.shuffled;  // different every time
    for (int i = 0; i <= max_var - 2; i++) {
      const int j = random.pick_int (i, max_var - 1);
      swap (shuffle[i], shuffle[j]);
    }
  } else {
    for (int idx = queue.last; idx; idx = links[idx].prev)
      shuffle.push_back (idx);
  }
  queue.first = queue.last = 0;
  for (const int idx : shuffle)
    queue.enqueue (links, idx);
  int64_t bumped = queue.bumped;
  for (int idx = queue.last; idx; idx = links[idx].prev)
    btab[idx] = bumped--;
  queue.unassigned = queue.last;
}

} // namespace CaDiCaL
