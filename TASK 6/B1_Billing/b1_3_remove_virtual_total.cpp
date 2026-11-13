// B1.3 - total() is NOT virtual: derived total() only HIDES the base one.
#include <iostream>
#include <string>
class Bill {
protected: std::string patient;
public:
    explicit Bill(const std::string& p) : patient(p) {}
    virtual ~Bill() = default;
    double total() const { return 0; }                    // virtual removed (body added so it compiles)
    virtual std::string type() const { return "Bill"; }
    void print() const { std::cout << type() << ' ' << patient << ": " << total() << '\n'; }
};
class OutPatient : public Bill {
    double consultation, tests;
public:
    OutPatient(const std::string& p, double c, double t) : Bill(p), consultation(c), tests(t) {}
    double total() const { return consultation + tests; }  // hides, does not override
    std::string type() const override { return "OP"; }
};
class InPatient : public Bill {
    int days; double perDay, surgery;
public:
    InPatient(const std::string& p, int d, double r, double s) : Bill(p), days(d), perDay(r), surgery(s) {}
    double total() const { return days * perDay + surgery; }
    std::string type() const override { return "IP"; }
};
int main()
{
    Bill* bills[] = {new OutPatient("Ravi", 500, 1200), new InPatient("Meena", 3, 2500, 40000)};
    double sum = 0;
    for (Bill* b : bills) { b->print(); sum += b->total(); }
    std::cout << "Total " << sum << '\n';
    for (Bill* b : bills) delete b;
}
