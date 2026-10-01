#include <iostream>
#include <vector>
#include <cstdlib> // for rand() function
#include <ctime> // for seeding rand() with time
/*
This creates a random array of 10 elements between -50 and 49, 
then finds the maximum sum of n consecutive elements, varying n */
using namespace std;

int sum(const vector<int>& v, int number, int start) {
    int total = 0;
    int cap;
    if (start + number > v.size()) {
        cap = v.size();
    }
    else {
        cap = start + number;
    }
    for (int i = start; i < cap; i++) {
        total += v[i];
    }
    return total;
}

int main(){
    srand(time(0)); // seed the random number generator
    vector<int> a(10);
    for (int i = 0; i < a.size(); i++){
        a[i] = rand() % 100 - 50; // random number between -50 and 49
    }
    cout << "Starting array: " << endl;
    for (int i = 0; i < a.size(); i++) {
        cout << a[i] << " ";
    }
    cout << endl;

    int n;
    int max;
    int n_max;
    int current_sum;
    int starting_index;
    /*// Version 1
    for (int i = 0; i < a.size(); i++) {
        for (n = 0; n <= a.size(); n++) {
            current_sum = sum(a, n, i);
            if (i == 0 && n == 0) {
                max = current_sum;
                n_max = n;
                starting_index = i;
            }
            else if (current_sum > max) {
                max = current_sum;
                n_max = n;
                starting_index = i;
            }
        }
    }
    */
    //Version 2
    for (int i = 0; i < a.size(); i++) {
        for (n = i; n < a.size(); n++) {
            if (i == 0 && n == 0) {
                max = a[0];
                current_sum = a[0];
                n_max = 1;
                starting_index = 0;
            }
            else {
                if (n==i) {
                    current_sum = a[i];
                }
                else {
                    current_sum += a[n];
                }
                if (current_sum > max) {
                    max = current_sum;
                    n_max = n - i + 1;
                    starting_index = i;
                }
            }
        }
    }
    /*Given the fact that once you have the segment with the maximum sum, 
    you can't expand it anymore, and that every subsegment of that segment will have a >0 sum
    (because if it had a negative sum, you could remove it and get a bigger sum),
    you can create a O(n) algorithm that finds the maximum sum of consecutive elements in a single pass through the array.
    Basically you start from the beginning of the array, and keep adding elements to the current sum (keeping track of the max). 
    If the current sum becomes negative, you reset it to 0 and start a new segment from the next element.
    */
    cout << "Maximum sum of " << n_max << " consecutive elements, starting at index " << starting_index << ": " << max << endl;
    return 0;
}