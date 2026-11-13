# Task A3 - Design with Abstract Classes (5 marks)
Design-only task; implementation in `../common/billing.hpp`, tests in `../B1_Billing/billing_full.cpp`, extension in `../B4_Extension/`.

- **A3.1 Payer interface:** `uml_payer.png` (source `uml_payer.dot`): abstract `Payer` with pure virtual `amountCovered(const Bill&)`; `SelfPay`, `Insurance` (limit, copay %), `GovernmentScheme` (fixed package rates).
- **A3.2 Extension:** `CorporateTieUp` derives from `Payer`; no existing class is edited.
```
ALGORITHM Insurance.AmountCovered(bill)
BEGIN
  1. share <- bill.total() * (100 - copayPercent) / 100
  2. RETURN MIN(coverLimit, share)
END
```
- **Alternative (std::function policies):** both O(1) per call; Insurance = 24 B, std::function = 32 B; limit hidden in the lambda capture. Full analysis: report section A3.
