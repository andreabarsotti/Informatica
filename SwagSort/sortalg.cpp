// compila con:  g++ -std=c++17 sortalg.cpp -o sortalg
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

// per creare numeri casuali
#include <random>
// per misurare i tempi di esecuzione
#include <chrono>

using namespace std;

/* UTILITY */

int random(int a, int b){ // restituisce un numero random in [a..b]
    static random_device rd;  // per inizializzare il generatore
    static mt19937 gen(rd()); // static evita di creare ogni volta il generatore a ogni chiamata 
    uniform_int_distribution<int> dist(a, b);

    return dist(gen);
}

vector<int> generaArrayCasuale(int n, int minVal = -100, int maxVal = 100){  // accetta solo n > 0 oppure tutti gli argomenti
    vector<int> A(n); // vettore di n interi da riempire
    for (auto i = 0; i < n; i++){
        A[i] = random(minVal,maxVal);
    }
    return A;  // non fa la copia!
}

void printArray(const vector<int>& B){
    for (auto x : B){
        cout << x << " ";
    }
    cout << endl;
}

bool verificaOrdinamento(const vector<int>& A){ // Theta(n)
    // A[i] <= A[i+1] per tutti i valori 0 ≤ i ≤ n-2
    // A[0] <= A[1] && A[1] <= A[2] && ... && A[n-2] <= A[n-1]
    bool risultato = true; // elemento neutro rispetto all'and logico &&
    for (auto i = 0; i + 1 < A.size(); ++i){
        risultato = risultato && A[i] <= A[i+1];
    }
    return risultato;    
}

bool verificaOrdinamento2(const vector<int>& A){ // O(n)
    for (auto i = 0; i + 1 < A.size(); ++i){
        if (A[i] > A[i+1]) return false;
    }
    return true;    
}

bool verificaOrdinamento3(const vector<int>& A){ // O(n)
    auto i = 0; 
    while (i + 1 < A.size() && A[i] <= A[i+1]) ++i;
    return i + 1 >= A.size();    
}

/* SELECTION SORT e INSERTION SORT */

void insertionSort(vector<int>& A){
    for (auto i = 1; i < A.size(); ++i){
        auto t = A[i];
        auto j = i;
        while (j > 0 && A[j-1] > t){
            A[j] = A[j-1];
            --j;
        }
        A[j] = t;
    }
}

/* QUICKSORT */

int Distribuzione(vector<int>& A, int sx, int px, int dx) {
    if (px != dx)
        swap(A[px], A[dx]);
    int i = sx;
    int j = dx - 1;
    while (i <= j) {
        while (i <= j && A[i] <= A[dx])
            i++;
        while (i <= j && A[j] >= A[dx])
            j--;
        if (i < j)
            swap(A[i], A[j]);
    }
    if (i != dx)
        swap(A[i], A[dx]);
    return i;
}


void QuickSortRandom(vector<int>& A, int sx, int dx, int L) {
    if (dx-sx >= L) {
        int pivot = random(sx, dx); 
        // cout << "---> sx = " << sx << " dx = " << dx << "valore pivot = " << A[pivot] << endl;
        // cout << "prima: ";
        // stampaArray(A, sx, dx);
        int rango = Distribuzione(A, sx, pivot, dx);
        // cout << "dopo: ";
        // stampaArray(A, sx, dx);
        QuickSortRandom(A, sx, rango - 1, L); // magia dell'induzione/ricorsione
        QuickSortRandom(A, rango + 1, dx, L);
    }
}

void QuickSortRandom(vector<int>& A, int L) { // wrapper
    QuickSortRandom(A, 0, A.size()-1, L);
}

void SwagSort(vector<int>& A, int L){
    QuickSortRandom(A, L);
    insertionSort(A);
}

/* MAIN */

int main()
{
    constexpr auto N = 1000;               // dimensione = numero di interi random
    int L = log(N); //lunghezza del segmento minimo accettato
    vector<int> A = generaArrayCasuale(N); // vector<int> A = {4, -6, 3, 5, -2, 1, -4, 6, -3};
    if (N < 100)
        printArray(A); // questa chiamata ignorata se N è grande


    auto ordinato = A;

    auto start = chrono::steady_clock::now();  // tempo d'inizio

    SwagSort(ordinato, L); // eseguiamo uno dei sort a turno sulla copia (perché?)

    auto stop = chrono::steady_clock::now();    // tempo di fine
    auto durata = chrono::duration_cast<chrono::microseconds>(stop - start);

    // printArray(B);
    if (!verificaOrdinamento(ordinato)) // fase di testing 
        cout << "Errore" << endl;

    cout << "\t" << durata.count() << " [microsecondi]" << endl;

    return 0;
}
   

    

