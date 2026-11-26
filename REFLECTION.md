# AuctionHub — Reflection

## 1. Why each algorithm fits its module

**A. Live Auction Monitor — Insertion Sort**
Bids arrive in near-sorted order, so each new key only needs to shift past a few elements (or none) to reach its place. On nearly-sorted data insertion sort runs close to its best case O(n) instead of O(n^2), and it is stable, so bids with the same amount keep their time order.

**B. Top-K Bid Finder — Selection Sort**
Each pass of selection sort (descending) finds the current maximum and puts it in front, so after k passes the top k bids are already in place. It works in-place with no extra memory and makes at most n − 1 swaps, which is fine for a small set.

**C. Price Anomaly Validator — Interchange Sort**
Batches are tiny (≤ 50 records), so O(n^2) means at most about 1,225 comparisons, which is instant. The algorithm is very simple to read and verify, which matters more than speed for a QA correctness check, and the swap count shows how far the batch was from sorted.

**D. Stabilization Detector — Bubble Sort (early stop)**
Bubble sort compares neighbouring bids, so a full pass with no swaps proves the stream is already in order. That first pass costs only n − 1 comparisons, O(n), which makes it a cheap "has the price stopped changing?" check, and the swapped flag stops the sort as soon as the data is ordered.

**E. Full Historical Sorter — Quick Sort**
Millions of bids need an O(n log n) algorithm; O(n^2) sorts would take hours. Quick sort is O(n log n) on average, sorts in-place, and median-of-three pivot selection avoids the worst case on sorted or reverse-sorted historical data.

## 2. Quick Sort runtime on 100k records

Quick sort (median-of-three) on 100,000 random bids: **11 ms** (measured with std::chrono, compiled with `clang++ -std=c++17 -O2` on macOS (Apple Silicon)). The result was verified with `isSortedBids` => Sorted: YES. See `screenshot_runtime.png`.

## 3. One bug I encountered

When I first ran the program, generating the 100,000-bid test file failed with a "cannot open file" error, so `test_data/large.txt` was never created and Task E had no data to sort. I asked the LLM to help diagnose it, and it explained that `ofstream` can create a new file but cannot create the folder it should go in: the `test_data` directory did not exist yet. I fixed it by creating the folder first with `mkdir test_data` in the Terminal; after that `large.txt` was generated successfully.

## 4. Three prompts I sent to the LLM
1. "Explain what this assignment is asking." — **Green** (asking for an explanation, not a solution).
2. "Review my code and fix it." — **Green / Yellow** (review of code I wrote myself; I asked for fixes, so I checked and understood every change before keeping it).
3. "Create 5 edge-case test inputs I should consider when testing." — **Green** (matches the allowed example "Generate 5 edge-case test inputs for sorting").

## 5. Worst case of Quick Sort with median-of-three

Yes, the worst case is still **O(n^2)**. Median-of-three only guarantees the pivot is not the smallest or largest of the three sampled elements; it says nothing about the other n − 3 elements. Specially built inputs ("median-of-three killer" sequences) can still make every partition very unbalanced, giving recursion depth close to n and about n^2/2 comparisons. In practice it removes the O(n^2) behaviour on sorted and reverse-sorted input, so the expected time is O(n log n), but the theoretical worst case does not change.

