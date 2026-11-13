// B1.1 - Hierarchy (A1) + payers (A3): one bill of each kind x each payer, checked by hand.
#include "billing.hpp"
#include <cmath>
#include <iomanip>

int main()
{
    std::unique_ptr<Bill> bills[] = {
        std::make_unique<OutPatient>("Ravi", 500, 1200),          // 1700
        std::make_unique<InPatient>("Meena", 3, 2500, 40000),     // 7500 + 40000 = 47500
        std::make_unique<DayCare>("Arun", 30000),                 // 30000
        std::make_unique<Emergency>("Latha", 2000, 4, 750)};      // 2000 + 3000 = 5000
    const double hand[4] = {1700, 47500, 30000, 5000};            // hand-verified totals

    SelfPay self;
    Insurance ins(40000, 10);                                     // limit 40000, copay 10 %
    GovernmentScheme gov({{"OP", 1000}, {"IP", 35000}, {"DC", 25000}, {"ER", 4000}});
    const Payer* payers[] = {&self, &ins, &gov};
    // hand-verified covered amounts [payer][bill]
    const double handCov[3][4] = {{0, 0, 0, 0},
                                  {1530, 40000, 27000, 4500},     // min(40000, 0.9*total)
                                  {1000, 35000, 25000, 4000}};    // min(total, package rate)

    int pass = 0, fail = 0;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Type Patient   Total      Payer      Covered    PatientPays  Check\n";
    for (int j = 0; j < 4; ++j)
        for (int i = 0; i < 3; ++i) {
            double t = bills[j]->total(), c = payers[i]->amountCovered(*bills[j]);
            bool ok = std::fabs(t - hand[j]) < 1e-9 && std::fabs(c - handCov[i][j]) < 1e-9;
            ok ? ++pass : ++fail;
            std::cout << std::left << std::setw(5) << bills[j]->type() << std::setw(9)
                      << bills[j]->patientName() << std::right << std::setw(10) << t << "  "
                      << std::left << std::setw(10) << payers[i]->name() << std::right
                      << std::setw(10) << c << std::setw(13) << t - c << "  "
                      << (ok ? "PASS" : "FAIL") << '\n';
        }
    Bill* raw[] = {bills[0].get(), bills[1].get(), bills[2].get(), bills[3].get()};
    std::cout << "Day total = " << dayTotal(raw, 4) << " (hand: 84200.00)\n";
    std::cout << "Passed " << pass << " / " << pass + fail << '\n';
    return fail == 0 ? 0 : 1;
}
