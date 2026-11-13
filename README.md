# DSA01 - Assignment 5 (CO5): Reliable Software with Runtime Polymorphism and Exceptions

| | |
|---|---|
| **Name** | Myyuri |
| **Register Number** | `<RegisterNumber>`  (fill in) |
| **Course** | DSA01 - Object Oriented Programming with C++ |
| **Case study** | Hospital billing and insurance claims |
| **Compiler** | g++ 13.3.0 (Ubuntu 24.04) |
| **Compile command** | `g++ -std=c++17 -Wall -Wextra` (tests add `-fsanitize=address -g`, timings add `-O2`) |
| **Report** | `Assignment5_CO5_Report.docx` (Part A design + Part B evidence) |

## How to build and run everything
```bash
bash run_all.sh            # compiles + runs every program, writes <task>/outputs/*.txt
python3 render_output.py   # (optional) turns outputs into <task>/screenshots/*.png  (needs Pillow)
```
Programs that use the shared header are compiled with `-I../common` from inside their own folder.

## Folder layout and how to run each task
| Folder | Files | Run (from inside the folder) |
|---|---|---|
| `common/` | `billing.hpp` - exceptions, `Bill` hierarchy, `Payer` hierarchy | header only |
| `A1_Hierarchy/` | `DESIGN.md` (interface, pseudocode), `uml_bill.png/.dot` | design only |
| `A2_RuntimeBinding/` | `vtable.png/.dot`, `predict_a2.cpp`, `predict_a2_nonvirtual_dtor.cpp`, `alt_dynamic_cast.cpp` | `g++ -std=c++17 -Wall -Wextra -fsanitize=address predict_a2.cpp -o a2 && ./a2` |
| `A3_Payer/` | `DESIGN.md`, `uml_payer.png/.dot` | design only |
| `A4_Exceptions/` | `DESIGN.md`, `exception_hierarchy.png/.dot` | design only |
| `A5_Recovery/` | `catch_flow.png/.dot`, `predict_a5.cpp`, `alt_manual_delete.cpp` | `g++ -std=c++17 -Wall -Wextra -fsanitize=address predict_a5.cpp -o a5 && ./a5` |
| `B1_Billing/` | `complete_b1.cpp` (Complete the Code), `billing_full.cpp` (B1.1), `b1_3_*.cpp` (remove virtual from total), `b1_4_*.cpp` (virtual destructor) | `g++ -std=c++17 -Wall -Wextra -fsanitize=address -I../common billing_full.cpp -o b1 && ./b1` |
| `B2_Errors/` | `predict_b2.cpp`, `complete_b2.cpp` (Complete the Code), `b2_throw_e.cpp`, `b2_throw_slicing_demo.cpp`, `error_tests.cpp`, `optional_bench.cpp` | `g++ -std=c++17 -Wall -Wextra -fsanitize=address -I../common error_tests.cpp -o et && ./et` |
| `B3_Benchmark/` | `correctness10.cpp` (B3.1), `bench.cpp` (B3.2, B3.3, variant) | `g++ -std=c++17 -O2 bench.cpp -o bench && ./bench` |
| `B4_Extension/` | `extension.cpp` - new bill type `Maternity` + new payer `CorporateTieUp`, no existing class edited | `g++ -std=c++17 -Wall -Wextra -fsanitize=address -g -I../common extension.cpp -o ext && ./ext` |

Every task folder also contains `outputs/` (console text of each run) and `screenshots/` (PNG of the same output).
The programs need no input files; the test data is inside the programs.

## Result of each task
| Task | Result |
|---|---|
| A2 predict | predicted `500 ~OPBill ~Bill` = actual. Non-virtual `~Bill`: only `~Bill` runs, compiler warning, **no** ASan report because `OPBill` adds no data members (same size as `Bill`) |
| A5 predict | predicted `~f ~main caught 42` = actual (stack unwinding order) |
| B1.1 | 4 bill kinds x 3 payers = 12/12 PASS against hand values; day total 84200 |
| B1 Complete the Code | expected output obtained: `OP Ravi: 1700`, `IP Meena: 47500`, `Total 49200` |
| B1.3 | removing `virtual` from the pure `total()` = compile errors; non-virtual `total()` with a body = every bill prints 0 |
| B1.4 | non-virtual destructor: ASan `new-delete-type-mismatch` (16 vs 8 bytes); virtual destructor: clean |
| B2 predict | predicted `ok done caught negative [log too large] caught too large` = actual |
| B2 Complete the Code | expected output obtained; `throw e;` slices when the handler catches a base class |
| B2 error tests | 6 deliberate errors + `catch (...)`: all handled, destructors ran, ASan clean (see note on `bad_alloc` in the report) |
| B3 | virtual vs switch vs variant vs per-type vectors: 10M bills = 131.6 / 94.5 / 95.1 / 22.5 ms; exceptions vs return codes at 50 % invalid: 497 ms vs 7.7 ms (65x) |
| B4 | new bill type + new payer added without editing `common/billing.hpp`; 12 earlier + 7 new tests = 20/20 PASS; ASan clean |

Timings were measured on one machine (1 core); numbers on your computer will differ, the ratios should be similar.

## Commit history (the brief requires progressive commits - at least one per task)
```bash
git init && git branch -M main
git add common README.md run_all.sh render_output.py && git commit -m "Common header, diagrams, README"
git add A1_Hierarchy A3_Payer A4_Exceptions && git commit -m "A1, A3, A4: designs, UML, exception hierarchy"
git add A2_RuntimeBinding   && git commit -m "A2: runtime binding, vtable, virtual destructor"
git add A5_Recovery  && git commit -m "A5: stack unwinding and RAII"
git add B1_Billing       && git commit -m "B1: polymorphic billing, remove-virtual experiments"
git add B2_Errors        && git commit -m "B2: error conditions, rethrow, optional"
git add B3_Benchmark     && git commit -m "B3: virtual vs switch vs variant, exceptions cost"
git add B4_Extension     && git commit -m "B4: new bill type and payer without changing old classes"
git remote add origin https://github.com/<username>/DSA01-<RegisterNumber>-Assignment5.git
git push -u origin main
```
(Make the commits while you re-run each task, so the history reflects your own work.)
