// B2 - deliberately broken inputs; every error is caught, logged and the program keeps running.
#include "billing.hpp"
#include <new>
struct Tracker {                                         // prints when destroyed (stack unwinding proof)
    std::string n;
    explicit Tracker(std::string s) : n(std::move(s)) {}
    ~Tracker() { std::cout << "   [~" << n << "]\n"; }
};
void test(const char* title, void (*fn)())
{
    std::cout << title << '\n';
    try { fn(); }
    catch (const NegativeChargeError& e) { std::cout << "   NegativeChargeError: " << e.what() << '\n'; }
    catch (const ClaimError& e)          { std::cout << "   ClaimError: " << e.what() << '\n'; }
    catch (const UnknownPackageError& e) { std::cout << "   UnknownPackageError: " << e.what() << '\n'; }
    catch (const RecordNotFoundError& e) { std::cout << "   RecordNotFoundError: " << e.what() << '\n'; }
    catch (const BillingError& e)        { std::cout << "   BillingError: " << e.what() << '\n'; }
    catch (const std::invalid_argument& e) { std::cout << "   std::invalid_argument: " << e.what() << '\n'; }
    catch (const std::bad_alloc& e)      { std::cout << "   std::bad_alloc: " << e.what() << '\n'; }
    catch (const std::exception& e)      { std::cout << "   std::exception: " << e.what() << '\n'; }
}
int main()
{
    test("1. Negative consultation charge", [] { Tracker t("t1"); OutPatient b("Ravi", -500, 1200); });
    test("2. Claim above the cover", [] { Tracker t("t2"); Insurance ins(40000, 10); ins.approveClaim(60000); });
    test("3. Unknown package code (government scheme has no ER rate)", [] {
        Tracker t("t3"); GovernmentScheme g({{"OP", 1000}});
        Emergency e("Latha", 2000, 4, 750); g.amountCovered(e); });
    test("4. Text where a number is expected", [] { Tracker t("t4"); double v = std::stod("abc"); (void)v; });
    test("5. Missing patient record", [] { Tracker t("t5"); PatientRegistry r; r.add("P1", "Ravi"); r.find("P9"); });
    test("6a. Impossible array length (bad_array_new_length, a std::bad_alloc)", [] {
        Tracker t("t6a"); std::size_t n = ~std::size_t(0) / 4;
        double* big = new double[n]; big[0] = 1; delete[] big; });
#ifndef __SANITIZE_ADDRESS__   // the ASan allocator aborts on absurd sizes instead of throwing
    test("6b. Memory allocation failure (8 TB request, normal build only)", [] {
        Tracker t("t6b"); std::size_t n = std::size_t(1) << 40;
        double* big = new double[n]; big[0] = 1; delete[] big; });
#endif
    std::cout << "7. Top-level catch(...) keeps the program running\n";
    try {
        Tracker outer("outer"); Tracker inner("inner");
        throw 3.14;                                      // not derived from std::exception
    } catch (...) { std::cout << "   catch(...) : unknown exception handled\n"; }
    std::cout << "Program still running - normal work resumes.\n";
    OutPatient ok("Ravi", 500, 1200); ok.print();
}
