#include <iostream>
#include <vector>
#include <cstdlib> // for rand() function
#include <random>
#include <chrono> // to measure 
using namespace std;
using namespace std::chrono;

/* --WHAT IT DOES--
This code creates a random array of 10 elements between -50 and 49, 
then finds the maximum sum of n consecutive elements, varying n.
If two segments produce the same sum, it chooses the one with the smallest n */

/* --HOW IT WORKS--
Once you have the segment with the maximum sum you can't expand it anymore, 
and every subsegment of that segment will have a >0 sum
(because if it had a negative sum, you could remove it and get a bigger sum),
So you start from the beginning of the array and keep adding elements to the current sum (keeping track of the max). 
If the current sum becomes negative, you reset it to 0 and start a new segment from the next element
*/

int random_gen(int a, int b){
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<int> dist(a,b);
    
    return dist(gen);
}

int main(){
    //creates the array
    int aa = -100;
    int bb = 100;
    int size = 10;
    vector<int> a(size);
    for (int i = 0; i < a.size(); i++){
        a[i] = random_gen(aa,bb);
    }
    
    //prints the starting array
    cout << "Starting array: " << endl;
    for (int i = 0; i < a.size(); i++) {
        cout << a[i] << " ";
    }
    cout << endl;

    //useful variables
    int n=0; //number of elements you are adding together
    int n_max = 0; //number of elements of the max segment
    int max; //maximum sum
    int current_sum =0; //current sum
    int starting_index; //starting index of the current segment
    int max_start_index; //starting index of the maximum segment
    
    auto start = high_resolution_clock::now(); //starts the chronometer

    for (int i=0; i<a.size(); i++){ //iterates through the array

        //adds the next element
        current_sum += a[i];
        n++;

        //first time?
        if (i==0){
            starting_index = i;
            max_start_index = i;
        }

        //bigger sum?
        if (current_sum > max){
            max = current_sum;
            n_max = n;
            max_start_index = starting_index;
        }

        //negative sum?
        if (current_sum <= 0){
            current_sum = 0;
            n=0;
            starting_index = i+1;
        }
    } 

    auto stop = high_resolution_clock::now(); //stops the chronometer
    auto durata = duration_cast<microseconds>(stop - start); //that's the duration baby

    cout << "Maximum sum of " << n_max << " consecutive elements, starting at index " << max_start_index << ": " << max << endl;
    cout << "Time of computation: " << durata.count() << endl;
    return 0;
}