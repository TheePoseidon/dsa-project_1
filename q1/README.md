# Question 1

## Warehouse Order Priority Sorter

Reads the e-commerce orders from a text file then sorts them by warehouse priority using a hand written quick dsort and writes the result on a new file.

## Files
- `company.c` — source code
- `orders.txt` — input file (one order per line: `OrderID CustomerName ProductCategory OrderValue`)
- `priority_orders.txt` — output file, created/overwritten each run

## Build
```bash
gcc -Wall -Wextra -Werror -pedantic company.c -o company_sort
```

## Run
```bash
./company
```
`orders.txt` must be in the same directory. The program prints the
number of orders processed and their total value, then writes the
sorted list to `priority_orders.txt`.

## Sort order
1. Order value — descending
2. Customer name — ascending (tiebreaker)
3. Order ID — ascending (final tiebreaker)

## Notes
- Quick Sort is implemented from scratch (no `qsort()`), using a
  randomized pivot to avoid worst-case behavior on already-sorted
  input.
- Average/best case: O(n log n). Worst case: O(n²) (rare, due to
  randomized pivoting). Auxiliary space: O(log n) average, O(n) worst
  case, from the recursion stack.