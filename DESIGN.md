# Task A4 - Design the Exception Strategy (4 marks)
Design-only task; implementation in `../common/billing.hpp`, tests in `../B2_Errors/`.

- **A4.1 Errors and hierarchy:** `exception_hierarchy.png` (source `exception_hierarchy.dot`).

| Error | Exception class |
|---|---|
| Negative charge | NegativeChargeError (BillingError) |
| Claim above cover | ClaimError (BillingError) |
| Unknown package | UnknownPackageError (BillingError) |
| Missing patient record | RecordNotFoundError (BillingError) |
| Text instead of number | std::invalid_argument |
| Allocation failure | std::bad_alloc |

- **A4.2 Specifications:** destructors and simple accessors are `noexcept`; validating constructors, `approveClaim`, `GovernmentScheme::amountCovered`, `PatientRegistry::find` may throw. Destructors must not throw: a second exception during unwinding calls `std::terminate`.
- **Alternative (return codes):** same speed with no errors (1.26 ms per 1M); 65x faster than exceptions at 50 % errors. Full analysis: report section A4.
