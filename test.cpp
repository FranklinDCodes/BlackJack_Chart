
#include <iostream>
#include <vector>

using std::cout, std::endl;
using std::vector;

enum myEnum {
    ZERO,
    ONE,
    TWO
};



int main() {

    myEnum x = ONE;

    vector<double> vec = {3, 4, 5, 6, 7};

    cout << vec.at(x) << endl;


}
