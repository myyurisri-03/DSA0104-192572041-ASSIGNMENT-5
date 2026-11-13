// B2 alternative: std::optional<double> vs exceptions for 1,000,000 claims
#include <chrono>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>
std::optional<double> approveOpt(double billed, double cover)
{ if (billed < 0 || billed > cover) return std::nullopt; return billed; }
double approveEx(double billed, double cover)
{ if (billed < 0 || billed > cover) throw std::invalid_argument("bad"); return billed; }
int main()
{
    const int N = 1000000;
    std::cout << "bad%   optional(ms)   exception(ms)   sizeof(optional<double>)=" << sizeof(std::optional<double>) << '\n';
    for (int pct : {0, 1, 50}) {
        std::vector<double> in(N); unsigned s = 1;
        for (int i = 0; i < N; ++i) { s = s * 1103515245u + 12345u; in[i] = ((s >> 16) % 100 < (unsigned)pct) ? -1.0 : 100.0; }
        using clk = std::chrono::steady_clock; double sumA = 0, sumB = 0, bestA = 1e9, bestB = 1e9; long badA = 0, badB = 0;
        for (int r = 0; r < 3; ++r) {
            auto t0 = clk::now(); sumA = 0; badA = 0;
            for (double x : in) { auto o = approveOpt(x, 50000); if (o) sumA += *o; else ++badA; }
            auto t1 = clk::now(); sumB = 0; badB = 0;
            for (double x : in) { try { sumB += approveEx(x, 50000); } catch (const std::exception&) { ++badB; } }
            auto t2 = clk::now();
            bestA = std::min(bestA, std::chrono::duration<double, std::milli>(t1 - t0).count());
            bestB = std::min(bestB, std::chrono::duration<double, std::milli>(t2 - t1).count());
        }
        std::cout << pct << "      " << bestA << "          " << bestB << "          (rejected " << badA << "/" << badB << ", sums " << sumA << "/" << sumB << ")\n";
    }
}
