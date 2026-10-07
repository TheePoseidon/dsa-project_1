# Laboratory Sample Queue (Doubly Linked List)

Models a hospital lab's sample-processing queue as a doubly linked
list, so a technician can step forward/backward through samples,
review the whole queue in either direction, and add new samples while
the program is running.

## Files
- `lab_sample.c` — source code (no input file needed; samples are
  entered interactively)

## Build
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c99 lab_sample.c -o lab_sample
```

## Run
```bash
./lab_sample
```
You'll first be asked how many initial samples to enter (0 is fine —
you can add more later from the menu), then details (ID, type,
priority: 1=Urgent, 2=Normal, 3=Routine) for each one.

## Menu
1. Move to next sample
2. Move to previous sample
3. Display current sample
4. Review queue forward (start to end)
5. Review queue backward (end to start)
6. Add new sample to end of queue
7. Exit

## Notes
- Every node is `malloc`'d and freed on exit — no leaks, no dangling
  pointers.
- Appending a new sample is O(1) (a tail pointer is maintained).
  Traversing all n samples in either direction is O(n).
- Handles an empty queue, a single-sample queue, and boundary moves
  (already at first/last) gracefully.
