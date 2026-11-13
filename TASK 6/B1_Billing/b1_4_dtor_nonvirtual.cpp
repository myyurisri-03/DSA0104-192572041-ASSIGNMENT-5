#include <iostream>
#include <string>
struct Bill { ~Bill() { std::cout << "~Bill\n"; } virtual double total() const = 0; };
struct InPatient : Bill {
    std::string* notes = new std::string(200, 'x');       // heap resource owned by the derived class
    ~InPatient() { delete notes; std::cout << "~InPatient\n"; }
    double total() const override { return 47500; }
};
int main() { Bill* b = new InPatient; std::cout << b->total() << '\n'; delete b; }
