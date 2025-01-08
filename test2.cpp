#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


int main() {

    const int REMOVE = 2;
    vector<int> vec;

    vector<int>::iterator maxQIterator = max_element(vec.begin(), vec.end());
    int maxQIndex = maxQIterator - vec.begin();

    cout << maxQIndex << endl;
    cout << vec.at(maxQIndex) << endl;




    return 0;
}
