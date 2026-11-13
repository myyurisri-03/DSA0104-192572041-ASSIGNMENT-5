# Task A1 - Design the Polymorphic Bill Hierarchy (7 marks)
Design-only task (pseudocode + UML); the implementation is in `../common/billing.hpp` and `../B1_Billing/`.

- **A1.1 Common interface:** `total()` (pure virtual), `type()` (virtual), `print()` (non-virtual), `~Bill()` (virtual); common data: `patient`.
- **A1.2 Hierarchy:** `uml_bill.png` (source `uml_bill.dot`). `Bill` is abstract; `OutPatient`, `InPatient`, `DayCare`, `Emergency` override `total()` and `type()`.
- **A1.3 Pseudocode**
```
ALGORITHM DayTotal(bills, n)
INPUT : bills - array of n Bill* pointers (bills of any kind)
OUTPUT: sum   - total amount of all bills of the day
BEGIN
  1. sum <- 0
  2. FOR i <- 0 TO n - 1 DO
  3.     sum <- sum + bills[i]->total()      // late binding picks the formula
  4. END FOR
  5. RETURN sum
END
```
- **Alternative (struct + type field + switch):** O(n) time in both; 32 B per bill for the switch struct vs vptr + fields + heap header for virtual; a new type needs edits in every switch. Full analysis: report section A1.
