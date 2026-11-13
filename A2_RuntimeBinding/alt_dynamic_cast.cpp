// A2 alternative: no virtual total(); type tested with dynamic_cast (up to k casts per bill)
#include <chrono>
#include <iostream>
#include <memory>
#include <vector>
struct Bill { virtual ~Bill() = default; virtual double total() const { return 0; } };   // virtual kept only for RTTI
struct OP : Bill { double t = 1700; double total() const override { return t; } };
struct IP : Bill { double t = 47500; };
struct DC : Bill { double t = 30000; };
struct ER : Bill { double t = 5000; };
struct XX : Bill { double t = 100; };
double viaCast(const Bill* b)
{
    if (auto p = dynamic_cast<const OP*>(b)) return p->t;
    if (auto p = dynamic_cast<const IP*>(b)) return p->t;
    if (auto p = dynamic_cast<const DC*>(b)) return p->t;
    if (auto p = dynamic_cast<const ER*>(b)) return p->t;
    if (auto p = dynamic_cast<const XX*>(b)) return p->t;
    return 0;
}
struct VBill { virtual ~VBill() = default; virtual double total() const = 0; };
struct VOP : VBill { double t = 1700;  double total() const override { return t; } };
struct VIP : VBill { double t = 47500; double total() const override { return t; } };
struct VDC : VBill { double t = 30000; double total() const override { return t; } };
struct VER : VBill { double t = 5000;  double total() const override { return t; } };
struct VXX : VBill { double t = 100;   double total() const override { return t; } };
int main()
{
    const int N = 2000000;
    std::vector<std::unique_ptr<Bill>> a; std::vector<std::unique_ptr<VBill>> v;
    unsigned s = 12345;
    for (int i = 0; i < N; ++i) {
        s = s * 1103515245u + 12345u; int k = (s >> 16) % 5;
        switch (k) { case 0: a.emplace_back(new OP); v.emplace_back(new VOP); break;
                     case 1: a.emplace_back(new IP); v.emplace_back(new VIP); break;
                     case 2: a.emplace_back(new DC); v.emplace_back(new VDC); break;
                     case 3: a.emplace_back(new ER); v.emplace_back(new VER); break;
                     default: a.emplace_back(new XX); v.emplace_back(new VXX); }
    }
    using clk = std::chrono::steady_clock;
    double best1 = 1e9, best2 = 1e9, s1 = 0, s2 = 0;
    for (int r = 0; r < 5; ++r) {
        auto t0 = clk::now(); s1 = 0; for (auto& p : a) s1 += viaCast(p.get());
        auto t1 = clk::now(); s2 = 0; for (auto& p : v) s2 += p->total();
        auto t2 = clk::now();
        best1 = std::min(best1, std::chrono::duration<double, std::milli>(t1 - t0).count());
        best2 = std::min(best2, std::chrono::duration<double, std::milli>(t2 - t1).count());
    }
    std::cout << "n = " << N << ", k = 5 bill types\n";
    std::cout << "dynamic_cast chain : " << best1 << " ms (sum " << s1 << ")\n";
    std::cout << "virtual call       : " << best2 << " ms (sum " << s2 << ")\n";
}
