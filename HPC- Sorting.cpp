#include <bits/stdc++.h>

using namespace std;
using namespace std::chrono;

// Sequential Bubble Sort
void bubbleSortSequential(vector<int> &arr)
{
    int n = arr.size();
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
}

// Parallel Bubble Sort using Odd-Even Transposition
void bubbleSortParallel(vector<int> &arr)
{
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        if (i % 2 == 0)
        {
// Even phase
#pragma omp parallel for
            for (int j = 0; j < n - 1; j += 2)
            {
                if (arr[j] > arr[j + 1])
                {
                    swap(arr[j], arr[j + 1]);
                }
            }
        }
        else
        {
// Odd phase
#pragma omp parallel for
            for (int j = 1; j < n - 1; j += 2)
            {
                if (arr[j] > arr[j + 1])
                {
                    swap(arr[j], arr[j + 1]);
                }
            }
        }
    }
}

// Sequential Merge Sort
void merge(vector<int> &arr, int l, int m, int r)
{
    int n1 = m - l + 1, n2 = r - m;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int i = 0; i < n2; i++)
        R[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1)
        arr[k++] = L[i++];
    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSortSequential(vector<int> &arr, int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSortSequential(arr, l, m);
        mergeSortSequential(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void mergeSortParallel(vector<int> &arr, int l, int r, int depth = 0)
{
    if (l < r)
    {
        int m = (l + r) / 2;
        if (depth <= 3)
        {
#pragma omp parallel sections
            {
#pragma omp section
                mergeSortParallel(arr, l, m, depth + 1);
#pragma omp section
                mergeSortParallel(arr, m + 1, r, depth + 1);
            }
        }
        else
        {
            mergeSortSequential(arr, l, m);
            mergeSortSequential(arr, m + 1, r);
        }
        merge(arr, l, m, r);
    }
}

void printArray(vector<int> &arr)
{
    for (auto x : arr)
        cout << x << " ";
    cout << endl;
}

// Main Function
int main()
{
    int size = 100000;
    vector<int> arr(size);
    for (int i = 0; i < size; i++)
        arr[i] = size - i;

    vector<int> arr1 = arr, arr2 = arr, arr3 = arr, arr4 = arr;

    // Bubble Sort Sequential
    auto start = high_resolution_clock::now();
    bubbleSortSequential(arr1);
    auto end = high_resolution_clock::now();
    duration<double> seqbubble = end - start;
    // printArray(arr1);
    cout << "Sequential Bubble Sort Time: " << seqbubble.count() << " seconds\n";

    // Bubble Sort Parallel
    start = high_resolution_clock::now();
    bubbleSortParallel(arr2);
    end = high_resolution_clock::now();
    duration<double> parbubble = end - start;
    // printArray(arr2);
    cout << "Parallel Bubble Sort Time: " << parbubble.count() << " seconds\n";

    // Merge Sort Sequential
    start = high_resolution_clock::now();
    mergeSortSequential(arr3, 0, size - 1);
    end = high_resolution_clock::now();
    duration<double> seqmerge = end - start;
    // printArray(arr3);
    cout << "Sequential Merge Sort Time: " << seqmerge.count() << " seconds\n";

    // Merge Sort Parallel
    start = high_resolution_clock::now();
    mergeSortParallel(arr4, 0, size - 1);
    end = high_resolution_clock::now();
    duration<double> parmerge = end - start;
    // printArray(arr4);
    cout << "Parallel Merge Sort Time: " << parmerge.count() << " seconds\n";

    return 0;
}
