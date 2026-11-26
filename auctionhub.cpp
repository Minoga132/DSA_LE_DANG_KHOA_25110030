#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
using namespace std::chrono;

struct Bid {
    string bidderId;
    double amount;
    long long timestamp;
    int auctionId;
};

void printBids(const vector<Bid>& v, size_t limit = 10) {
    for (size_t i = 0; i < min(v.size(), limit); ++i)
        cout << "[" << v[i].bidderId << "] $" << v[i].amount << " @ " << v[i].timestamp << "\n";
    if (v.size() > limit) cout << "... (" << v.size() << " total)\n";
}

bool loadBids(const string& path, vector<Bid>& out) {
    ifstream f(path);
    if (!f) return false;
    Bid b;
    while (f >> b.bidderId >> b.amount >> b.timestamp >> b.auctionId)
        out.push_back(b);
    return true;
}

bool sosanhBids(const Bid& a, const Bid& b) {
    if (a.amount == b.amount) {
        return a.timestamp < b.timestamp;
    }
    return a.amount < b.amount;
}

bool isSortedBids(const vector<Bid>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (sosanhBids(v[i], v[i - 1])) return false;
    }
    return true;
}

void insertionSort(vector<Bid>& v) {
    for (int i = 1; i < (int)v.size(); ++i) {
        Bid k = v[i];
        int j = i - 1;
        while (j >= 0 && sosanhBids(k, v[j])) {
            v[j + 1] = v[j];
            j = j - 1;
        }
        v[j + 1] = k;
    }
}

void selectionSort(vector<Bid>& v) {
    int n = v.size();
    for (int i = 0; i < n - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (v[j].amount > v[maxIdx].amount) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) swap(v[i], v[maxIdx]);
    }
}

int interchangeSort(vector<Bid>& v) {
    int count = 0;
    int n = v.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (v[i].amount > v[j].amount) {
                swap(v[i], v[j]);
                count++;
            }
        }
    }
    return count;
}

bool bubbleSortEarlyStop(vector<Bid>& v) {
    int n = v.size();
    bool stable = true;
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            if (v[j].amount > v[j + 1].amount) {
                swap(v[j], v[j + 1]);
                swapped = true;
            }
        }
        if (i == 0 && swapped) stable = false;
        if (!swapped) break;
    }
    return stable;
}

void quickSort(vector<Bid>& v, int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;
    if (sosanhBids(v[mid], v[left])) swap(v[left], v[mid]);
    if (sosanhBids(v[right], v[left])) swap(v[left], v[right]);
    if (sosanhBids(v[right], v[mid])) swap(v[mid], v[right]);
    swap(v[mid], v[right]);

    Bid pivot = v[right];
    int i = left;
    for (int j = left; j < right; j++) {
        if (sosanhBids(v[j], pivot)) {
            swap(v[i], v[j]);
            i++;
        }
    }
    swap(v[i], v[right]);

    quickSort(v, left, i - 1);
    quickSort(v, i + 1, right);
}

void quickSortWrapper(vector<Bid>& v) {
    if (v.empty()) return;
    quickSort(v, 0, (int)v.size() - 1);
}

void generateLargeTestFile(const string& filename, int num_records) {
    ofstream out(filename);
    if (!out) {
        cout << "Loi: Khong the tao file " << filename << " (thu muc test_data da ton tai chua?)\n";
        return;
    }
    mt19937 gen(12345);
    uniform_real_distribution<double> dis_amount(10.0, 5000.0);
    uniform_int_distribution<long long> dis_time(1600000000000LL, 1700000000000LL);
    uniform_int_distribution<int> dis_auction(1, 50);
    for (int i = 0; i < num_records; ++i) {
        out << "bidder_" << i << " "
            << dis_amount(gen) << " "
            << dis_time(gen) << " "
            << dis_auction(gen) << "\n";
    }
}

int main() {
    vector<Bid> small;
    if (!loadBids("test_data/small.txt", small)) {
        cout << "Khong doc duoc test_data/small.txt\n";
        return 1;
    }

    vector<Bid> a = small;
    insertionSort(a);
    printBids(a);

    vector<Bid> b = small;
    selectionSort(b);
    printBids(b, 3);

    vector<Bid> c = small;
    int swaps = interchangeSort(c);
    printBids(c);
    cout << "Swaps: " << swaps << "\n";

    vector<Bid> sortedInput = a;
    vector<Bid> reversedInput(a.rbegin(), a.rend());
    cout << "Sorted input:   " << (bubbleSortEarlyStop(sortedInput) ? "STABLE" : "UNSTABLE") << "\n";
    cout << "Reversed input: " << (bubbleSortEarlyStop(reversedInput) ? "STABLE" : "UNSTABLE") << "\n";

    generateLargeTestFile("test_data/large.txt", 100000);
    vector<Bid> bids;
    if (!loadBids("test_data/large.txt", bids)) {
        cout << "Khong doc duoc test_data/large.txt\n";
        return 1;
    }
    auto start = high_resolution_clock::now();
    quickSortWrapper(bids);
    auto end = high_resolution_clock::now();
    cout << "Runtime: " << duration_cast<milliseconds>(end - start).count() << " ms\n";
    cout << "Sorted: " << (isSortedBids(bids) ? "YES" : "NO") << "\n";
    printBids(bids, 5);

    return 0;
}
