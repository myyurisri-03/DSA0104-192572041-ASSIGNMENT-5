// B3.1 - the same 10 bills totalled with (a) virtual functions and (b) struct + switch
#include "billing.hpp"
#include <cmath>
struct SBill { int type; double a, b, c; };               // 0=OP 1=IP 2=DC 3=ER
static double totalSwitch(const SBill& x)
{
    switch (x.type) {
        case 0: return x.a + x.b;
        case 1: return x.a * x.b + x.c;
        case 2: return x.a;
        default: return x.a + x.b * x.c;
    }
}
int main()
{
    std::vector<std::unique_ptr<Bill>> v;
    v.emplace_back(new OutPatient("P1", 500, 1200));  v.emplace_back(new OutPatient("P2", 300, 0));
    v.emplace_back(new InPatient("P3", 3, 2500, 40000)); v.emplace_back(new InPatient("P4", 2, 3000, 0));
    v.emplace_back(new InPatient("P5", 5, 1800, 12000));
    v.emplace_back(new DayCare("P6", 30000));         v.emplace_back(new DayCare("P7", 18500));
    v.emplace_back(new Emergency("P8", 2000, 4, 750)); v.emplace_back(new Emergency("P9", 1500, 2, 600));
    v.emplace_back(new Emergency("P10", 2500, 0.5, 800));
    // days of the IP bills are stored in field a, rate in b (same layout as bench.cpp)
    std::vector<SBill> s = {{0, 500, 1200, 0}, {0, 300, 0, 0}, {1, 3, 2500, 40000}, {1, 2, 3000, 0}, {1, 5, 1800, 12000},
                            {2, 30000, 0, 0}, {2, 18500, 0, 0}, {3, 2000, 4, 750}, {3, 1500, 2, 600}, {3, 2500, 0.5, 800}};
    const double hand[10] = {1700, 300, 47500, 6000, 21000, 30000, 18500, 5000, 2700, 2900};   // by hand
    double sv = 0, ss = 0, sh = 0; int ok = 0;
    std::cout << "No  Type  Hand      Virtual   Switch    Match\n";
    for (int i = 0; i < 10; ++i) {
        double a = v[i]->total(), b = totalSwitch(s[i]);
        bool m = a == hand[i] && b == hand[i]; ok += m;
        sv += a; ss += b; sh += hand[i];
        std::printf("%-3d %-5s %-9g %-9g %-9g %s\n", i + 1, v[i]->type().c_str(), hand[i], a, b, m ? "yes" : "NO");
    }
    std::printf("Sum: hand=%g virtual=%g switch=%g  -> %s (%d/10 bills agree)\n", sh, sv, ss,
                (sh == sv && sv == ss) ? "BOTH AGREE" : "MISMATCH", ok);
}
