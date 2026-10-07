# E-Sports Tournament Bracket Tree

Builds a single-elimination tournament bracket as a binary tree from a
list of 69 participant identifiers (hard-coded in the source), then
lets an administrator explore relationships between participants and
add new entrants at runtime.

## Files
- `sport_tournament.c` — source code (no input file needed; the
  participant list is built into the program)

## Build
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c99 sport_tournament.c -o tournament_tree
```

## Run
```bash
./tournament_tree
```
The bracket is built automatically on startup.

## Menu
1. Display root match/participant
2. Display all leaf participants
3. Look up a participant (shows parent, sibling, grandchildren)
4. Add a new participant to the tournament
5. Exit

## How the tree is built
Leaves = the 69 participants, in input order. Each round, adjacent
nodes pair up under a new "match" node; an odd node out gets a bye and
advances unchanged. This repeats until one root (the final match)
remains — a full binary tree (every internal node has exactly 2
children).

## Notes
- Searching/traversal is O(n) (recursive tree walk).
- Adding a new participant is O(n): it does a level-order search for
  the shallowest participant (the one who's played fewest rounds) and
  pairs them against the newcomer. See the in-file comments for the
  full justification of this cost.
