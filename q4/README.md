# Pharmacy Inventory (Binary Search Tree)

Loads a hospital pharmacy's medicine inventory from a pipe-delimited
text file into a Binary Search Tree keyed by medicine code, so staff
can search by code and list the inventory in sorted order without
rescanning the file each time.

## Files
- `pharmacy.c` — source code
- `medicines.txt` — input file, one record per line:
  `MedicineCode|MedicineName|Quantity|UnitPrice`

## Build
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c99 pharmacy.c -o pharmacy
```

## Run
```bash
./pharmacy
```
You'll be prompted for the file path — press Enter to use the default
(`medicines.txt` in the current directory).

## Menu
1. Search for a medicine by code
2. Display full inventory (sorted by code)
3. Exit

## Validation
Blank lines are skipped silently. Malformed lines (missing/empty
fields, non-numeric or negative quantity/price, extra fields, lines
that are too long) are skipped with a warning naming the line number
and reason. Duplicate medicine codes update the existing node's
quantity rather than creating a second node. A load summary (lines
read, inserted, updated, skipped) prints on startup.

## Notes
- Search is O(h), where h is tree height: O(log n) average if the
  tree is balanced, but O(n) worst case — notably, inserting
  already-sorted codes (as in the sample file) produces exactly that
  worst-case shape.
