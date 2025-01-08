
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int DIM_0_SIZE = 24;
const int DIM_1_SIZE = 12;
const int DIM_2_SIZE = 4;

int main() {

    double table[DIM_0_SIZE][DIM_1_SIZE][DIM_2_SIZE];

    for (int i = 0; i < DIM_0_SIZE; i ++) {
        for (int j = 0; j < DIM_1_SIZE; j ++) {
            for (int k = 0; k < DIM_2_SIZE; k ++) {
                table[i][j][k] = 1;
            }
        }
    }

    int ind0 = 20;
    int ind1 = 10;

    double* maxPtr = max_element(table[ind0][ind1], table[ind0][ind1] + DIM_2_SIZE);
    int maxIdx = maxPtr - table[ind0][ind1];

    cout << maxIdx << endl;

    return 0;
}


