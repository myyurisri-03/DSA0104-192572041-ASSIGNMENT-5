// A5 alternative: manual delete in every catch block instead of RAII.
#include <iostream>
#include <stdexcept>
struct Res { Res() { std::cout << "acquire "; } ~Res() { std::cout << "release "; } };
void work(bool failA, bool failB)
{
    Res* a = new Res;                                   // leak point 1
    try {
        if (failA) throw std::runtime_error("A");
        Res* b = new Res;                               // leak point 2
        try {
            if (failB) throw std::runtime_error("B");
        } catch (...) { delete b; throw; }             // must be repeated on every path
        delete b;
    } catch (...) { delete a; throw; }                 // ... and here
    delete a;
}
int main()
{
    for (int i = 0; i < 3; ++i)
        try { work(i == 1, i == 2); std::cout << "ok\n"; }
        catch (const std::exception& e) { std::cout << "caught " << e.what() << '\n'; }
}
