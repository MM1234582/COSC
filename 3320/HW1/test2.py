import collections
import itertools
import random
import time

TestArray = collections.namedtuple("TestArray", ["array", "type"])


def test_arrays(n):
    reverse_sorted = TestArray(list(reversed(range(1, n + 1))), "reverse sorted")
    randomized = TestArray(random.sample(list(range(1, n + 1)), k=n), "randomized")
    odd_even = TestArray(list(range(1, n + 1, 2)) + list(range(2, n + 1, 2)), "odd even")
    return reverse_sorted, randomized, odd_even

def quick_sort(arr):
    print("Hello 2\n")

def quick_sort_randomized(arr):
    print("Hello\n")

def merged(left, right):
    merged_array = []
    m, n = len(left), len(right)
    i, j = 0, 0
    while i < m and j < n:
        if left[i] <= right[j]:
            merged_array.append(left[i])
            i += 1
        else:
            merged_array.append(right[j])
            j += 1
    if i == m:  # merged all elements from left
        merged_array.extend(right[j:n])  # Fix the typo here
    else:  # merged all elements from right
        merged_array.extend(left[i:m])   # Fix the typo here
    return merged_array

def merge_sort(arr):
    n = len(arr)
    if n <= 1:
        return arr
    else:
        mid = n // 2  # integer division
        left = arr[0:mid]
        right = arr[mid:n]
        sorted_left, sorted_right = merge_sort(left), merge_sort(right)
        return merged(sorted_left, sorted_right)

def main():
    print("n,array type,sorting algorithm,time")
    for n in [2**i for i in range(5, 10)]:
        reverse_sorted, randomized, odd_even = test_arrays(n)
        funcs = [
            merge_sort,
        ]
        arrays = [reverse_sorted, randomized, odd_even]
        for func, array in itertools.product(funcs, arrays):
            start_time = time.time()
            func(array.array)
            total_time = time.time() - start_time
            print(f"{n},{array.type},{func.__name__},{total_time:0.6f}")
    return 0


if __name__ == "__main__":
    main()
