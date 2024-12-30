
#include <iostream>
#include <vector>

using std::cout, std::endl;
using std::vector;




int main() {

    vector<int> myVec = {0, 1, 2, 3};

    cout << myVec.at(-1) << endl;
    // DOES NOT WORK

}
