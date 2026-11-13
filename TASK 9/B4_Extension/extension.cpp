// B4.1 / B4.2 - ONE new bill type (Maternity) and ONE new payer (CorporateTieUp)
// added WITHOUT editing billing.hpp (Bill, Payer, SelfPay, Insurance, GovernmentScheme untouched).
#include "billing.hpp"
#include <cmath>
#include <iomanip>

// ---- new bill type ----------------------------------------------------------
class Maternity : public Bill {
    double packagePrice; int days; double roomPerDay;
public:
    Maternity(const std::string& p, double pkg, int d, double room)
        : Bill(p), packagePrice(nonNeg(pkg, "package")), days(d), roomPerDay(nonNeg(room, "room rate"))
    { if (d < 0) throw NegativeChargeError("negative days"); }
    double total() const override { return packagePrice + days * roomPerDay; }
    std::string type() const override { return "MT"; }
};
// ---- new payer --------------------------------------------------------------
class CorporateTieUp : public Payer {
    double sharePct, ceiling;                              // company pays sharePct %, up to ceiling
public:
    CorporateTieUp(double pct, double cap) : sharePct(pct), ceiling(cap) {}
    double amountCovered(const Bill& b) const override { return std::min(ceiling, b.total() * sharePct / 100); }
    std::string name() const override { return "Corporate"; }
};

struct Case { const Bill* bill; const Payer* payer; double expect; bool expectThrow; };

int main()
{
    OutPatient op("Ravi", 500, 1200); InPatient ip("Meena", 3, 2500, 40000);
    DayCare dc("Arun", 30000);        Emergency er("Latha", 2000, 4, 750);
    Maternity mt("Divya", 45000, 3, 2000);                                   // 45000 + 6000 = 51000
    SelfPay self; Insurance ins(40000, 10);
    GovernmentScheme gov({{"OP", 1000}, {"IP", 35000}, {"DC", 25000}, {"ER", 4000}});
    CorporateTieUp corp(80, 30000);

    std::vector<Case> cases = {
        // --- the 12 original tests (hand values from B1) ---
        {&op, &self, 0, 0}, {&op, &ins, 1530, 0}, {&op, &gov, 1000, 0},
        {&ip, &self, 0, 0}, {&ip, &ins, 40000, 0}, {&ip, &gov, 35000, 0},
        {&dc, &self, 0, 0}, {&dc, &ins, 27000, 0}, {&dc, &gov, 25000, 0},
        {&er, &self, 0, 0}, {&er, &ins, 4500, 0}, {&er, &gov, 4000, 0},
        // --- new bill with old payers ---
        {&mt, &self, 0, 0}, {&mt, &ins, 40000, 0}, {&mt, &gov, 0, 1},          // gov has no MT package -> exception
        // --- new payer with all bills ---
        {&op, &corp, 1360, 0}, {&ip, &corp, 30000, 0}, {&dc, &corp, 24000, 0},
        {&er, &corp, 4000, 0}, {&mt, &corp, 30000, 0}};

    int pass = 0, fail = 0, idx = 0;
    std::cout << std::fixed << std::setprecision(2);
    for (const Case& c : cases) {
        ++idx; bool ok = false; std::string note;
        try {
            double got = c.payer->amountCovered(*c.bill);
            ok = !c.expectThrow && std::fabs(got - c.expect) < 1e-9;
            note = std::to_string(got);
        } catch (const UnknownPackageError& e) {
            ok = c.expectThrow; note = std::string("UnknownPackageError caught: ") + e.what();
        }
        ok ? ++pass : ++fail;
        std::cout << std::right << std::setw(2) << idx << ". " << c.bill->type() << " x " << std::left
                  << std::setw(10) << c.payer->name() << " -> " << note << "  " << (ok ? "PASS" : "FAIL") << '\n';
    }
    std::cout << "Passed " << pass << " / " << pass + fail << " (12 earlier tests + 7 new)\n";
    return fail == 0 ? 0 : 1;
}
