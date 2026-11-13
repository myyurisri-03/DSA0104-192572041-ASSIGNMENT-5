// B3 - cost of virtual functions vs switch vs std::variant vs per-type containers,
//      and return codes vs exceptions.  Build: g++ -std=c++17 -O2 bench.cpp -o bench
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <variant>
#include <vector>
using clk = std::chrono::steady_clock;
static double ms(clk::time_point a, clk::time_point b) { return std::chrono::duration<double, std::milli>(b - a).count(); }

// ---- (a) virtual hierarchy ------------------------------------------------
struct VBill { virtual ~VBill() = default; virtual double total() const = 0; };
struct VOP : VBill { double c, t; VOP(double a, double b) : c(a), t(b) {} double total() const override { return c + t; } };
struct VIP : VBill { int d; double r, s; VIP(int a, double b, double c2) : d(a), r(b), s(c2) {} double total() const override { return d * r + s; } };
struct VDC : VBill { double p; explicit VDC(double a) : p(a) {} double total() const override { return p; } };
struct VER : VBill { double b, h, r; VER(double a, double c2, double d2) : b(a), h(c2), r(d2) {} double total() const override { return b + h * r; } };
// ---- (b) one struct + type field + switch ----------------------------------
struct SBill { int type; double a, b, c; };
static double totalSwitch(const SBill& x)
{
    switch (x.type) {
        case 0: return x.a + x.b;
        case 1: return x.a * x.b + x.c;
        case 2: return x.a;
        default: return x.a + x.b * x.c;
    }
}
// ---- (c) std::variant -------------------------------------------------------
struct POP { double c, t; double total() const { return c + t; } };
struct PIP { int d; double r, s; double total() const { return d * r + s; } };
struct PDC { double p; double total() const { return p; } };
struct PER { double b, h, r; double total() const { return b + h * r; } };
using Var = std::variant<POP, PIP, PDC, PER>;

static unsigned rng(unsigned& s) { s = s * 1103515245u + 12345u; return s >> 16; }

static void sizes(std::size_t n)
{
    std::vector<std::unique_ptr<VBill>> v; v.reserve(n);
    std::vector<SBill> s; s.reserve(n);
    std::vector<Var> w; w.reserve(n);
    std::vector<POP> o; std::vector<PIP> i; std::vector<PDC> d; std::vector<PER> e;
    unsigned seed = 7;
    for (std::size_t k = 0; k < n; ++k) {
        int t = rng(seed) % 4; double a = 100 + rng(seed) % 900, b = 1 + rng(seed) % 50, c = 10 + rng(seed) % 90;
        switch (t) {
            case 0: v.emplace_back(new VOP(a, b)); s.push_back({0, a, b, 0}); w.emplace_back(POP{a, b}); o.push_back({a, b}); break;
            case 1: v.emplace_back(new VIP(int(b), a, c)); s.push_back({1, b, a, c}); w.emplace_back(PIP{int(b), a, c}); i.push_back({int(b), a, c}); break;
            case 2: v.emplace_back(new VDC(a)); s.push_back({2, a, 0, 0}); w.emplace_back(PDC{a}); d.push_back({a}); break;
            default: v.emplace_back(new VER(a, b, c)); s.push_back({3, a, b, c}); w.emplace_back(PER{a, b, c}); e.push_back({a, b, c});
        }
    }
    const int reps = n >= 10000000 ? 3 : (n >= 1000000 ? 5 : 2000);
    double bv = 1e18, bs = 1e18, bw = 1e18, bc = 1e18, sv = 0, ss = 0, sw = 0, sc = 0;
    for (int r = 0; r < reps; ++r) {
        auto t0 = clk::now(); sv = 0; for (auto& p : v) sv += p->total();
        auto t1 = clk::now(); ss = 0; for (auto& x : s) ss += totalSwitch(x);
        auto t2 = clk::now(); sw = 0; for (auto& x : w) sw += std::visit([](const auto& q) { return q.total(); }, x);
        auto t3 = clk::now(); sc = 0;
        for (auto& x : o) sc += x.total();
        for (auto& x : i) sc += x.total();
        for (auto& x : d) sc += x.total();
        for (auto& x : e) sc += x.total();
        auto t4 = clk::now();
        bv = std::min(bv, ms(t0, t1)); bs = std::min(bs, ms(t1, t2)); bw = std::min(bw, ms(t2, t3)); bc = std::min(bc, ms(t3, t4));
    }
    std::printf("%-10zu %12.3f %12.3f %12.3f %14.3f   %s\n", n, bv, bs, bw, bc,
                (sv == ss && ss == sw && std::abs(sc - sv) < 1e-6 * sv) ? "sums agree" : "SUMS DIFFER");
}

// ---- validation: return codes vs exceptions -----------------------------------
enum Status { OK, NEGATIVE };
static Status validateRC(const SBill& x) { return (x.a < 0 || x.b < 0) ? NEGATIVE : OK; }
static void validateEx(const SBill& x) { if (x.a < 0 || x.b < 0) throw std::invalid_argument("negative"); }

static void validation()
{
    const int N = 1000000;
    std::printf("\n%-8s %18s %18s %10s\n", "invalid%", "return codes (ms)", "exceptions (ms)", "ratio");
    for (int pct : {0, 1, 50}) {
        std::vector<SBill> v(N); unsigned seed = 99;
        for (auto& x : v) { x = {0, 500.0, 1200.0, 0}; if ((int)(rng(seed) % 100) < pct) x.a = -1; }
        double br = 1e18, be = 1e18; long cr = 0, ce = 0;
        for (int r = 0; r < 3; ++r) {
            auto t0 = clk::now(); cr = 0; for (auto& x : v) if (validateRC(x) != OK) ++cr;
            auto t1 = clk::now(); ce = 0; for (auto& x : v) { try { validateEx(x); } catch (const std::exception&) { ++ce; } }
            auto t2 = clk::now();
            br = std::min(br, ms(t0, t1)); be = std::min(be, ms(t1, t2));
        }
        std::printf("%-8d %18.3f %18.3f %9.1fx   (invalid found %ld / %ld)\n", pct, br, be, be / br, cr, ce);
    }
}

int main()
{
    std::printf("sizeof: VOP=%zu VIP=%zu VDC=%zu VER=%zu | SBill=%zu | variant=%zu (POP=%zu PIP=%zu PDC=%zu PER=%zu)\n",
                sizeof(VOP), sizeof(VIP), sizeof(VDC), sizeof(VER), sizeof(SBill), sizeof(Var),
                sizeof(POP), sizeof(PIP), sizeof(PDC), sizeof(PER));
    std::printf("\nTotal time for one pass over n bills (best of several runs), -O2\n");
    std::printf("%-10s %12s %12s %12s %14s\n", "n", "virtual(ms)", "switch(ms)", "variant(ms)", "per-type(ms)");
    for (std::size_t n : {1000ul, 1000000ul, 10000000ul}) sizes(n);
    validation();
}
