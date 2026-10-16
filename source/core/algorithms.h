#pragma once
#include <algorithm>
#include <iterator>
#include <numeric>
#include <utility>

namespace Khan
{
    // Searching
    template<typename TIterator, typename TValue>
    [[nodiscard]] TIterator Find(TIterator first, TIterator last, const TValue& value)
    {
        return std::find(first, last, value);
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] TIterator FindIf(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::find_if(first, last, std::move(predicate));
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] TIterator FindIfNot(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::find_if_not(first, last, std::move(predicate));
    }

    template<typename TIterator, typename TValue>
    [[nodiscard]] bool Contains(TIterator first, TIterator last, const TValue& value)
    {
        return std::find(first, last, value) != last;
    }

    template<typename TIterator, typename TValue, typename TBinaryPredicate>
    [[nodiscard]] bool Contains(TIterator first, TIterator last, const TValue& value, TBinaryPredicate predicate)
    {
        return std::find_if(first, last, [&](const auto& element) { return predicate(element, value); }) != last;
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] TIterator FindFirstOf(TIterator first, TIterator last, TIterator searchFirst, TIterator searchLast, TPredicate predicate)
    {
        return std::find_first_of(first, last, searchFirst, searchLast, std::move(predicate));
    }

    template<typename TIterator, typename TValue>
    [[nodiscard]] std::size_t Count(TIterator first, TIterator last, const TValue& value)
    {
        return static_cast<std::size_t>(std::count(first, last, value));
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] std::size_t CountIf(TIterator first, TIterator last, TPredicate predicate)
    {
        return static_cast<std::size_t>(std::count_if(first, last, std::move(predicate)));
    }

    // Querying
    template<typename TIterator, typename TPredicate>
    [[nodiscard]] bool AnyOf(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::any_of(first, last, std::move(predicate));
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] bool AllOf(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::all_of(first, last, std::move(predicate));
    }

    template<typename TIterator, typename TPredicate>
    [[nodiscard]] bool NoneOf(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::none_of(first, last, std::move(predicate));
    }

    template<typename TIterator>
    [[nodiscard]] bool IsSorted(TIterator first, TIterator last)
    {
        return std::is_sorted(first, last);
    }

    template<typename TIterator, typename TCompare>
    [[nodiscard]] bool IsSorted(TIterator first, TIterator last, TCompare compare)
    {
        return std::is_sorted(first, last, std::move(compare));
    }

    // Sorting
    template<typename TIterator>
    void Sort(TIterator first, TIterator last)
    {
        std::sort(first, last);
    }

    template<typename TIterator, typename TCompare>
    void Sort(TIterator first, TIterator last, TCompare compare)
    {
        std::sort(first, last, std::move(compare));
    }

    template<typename TIterator>
    void StableSort(TIterator first, TIterator last)
    {
        std::stable_sort(first, last);
    }

    template<typename TIterator, typename TCompare>
    void StableSort(TIterator first, TIterator last, TCompare compare)
    {
        std::stable_sort(first, last, std::move(compare));
    }

    template<typename TIterator>
    void PartialSort(TIterator first, TIterator middle, TIterator last)
    {
        std::partial_sort(first, middle, last);
    }

    template<typename TIterator, typename TCompare>
    void PartialSort(TIterator first, TIterator middle, TIterator last, TCompare compare)
    {
        std::partial_sort(first, middle, last, std::move(compare));
    }

    template<typename TIterator, typename TValue>
    TIterator Partition(TIterator first, TIterator last, TValue predicate)
    {
        return std::partition(first, last, std::move(predicate));
    }

    template<typename TIterator, typename TPredicate>
    TIterator StablePartition(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::stable_partition(first, last, std::move(predicate));
    }

    // Binary Search (requires sorted input)
    template<typename TIterator, typename TValue>
    [[nodiscard]] bool BinarySearch(TIterator first, TIterator last, const TValue& value)
    {
        return std::binary_search(first, last, value);
    }

    template<typename TIterator, typename TValue, typename TCompare>
    [[nodiscard]] bool BinarySearch(TIterator first, TIterator last, const TValue& value, TCompare compare)
    {
        return std::binary_search(first, last, value, std::move(compare));
    }

    template<typename TIterator, typename TValue>
    [[nodiscard]] TIterator LowerBound(TIterator first, TIterator last, const TValue& value)
    {
        return std::lower_bound(first, last, value);
    }

    template<typename TIterator, typename TValue, typename TCompare>
    [[nodiscard]] TIterator LowerBound(TIterator first, TIterator last, const TValue& value, TCompare compare)
    {
        return std::lower_bound(first, last, value, std::move(compare));
    }

    template<typename TIterator, typename TValue>
    [[nodiscard]] TIterator UpperBound(TIterator first, TIterator last, const TValue& value)
    {
        return std::upper_bound(first, last, value);
    }

    template<typename TIterator, typename TValue, typename TCompare>
    [[nodiscard]] TIterator UpperBound(TIterator first, TIterator last, const TValue& value, TCompare compare)
    {
        return std::upper_bound(first, last, value, std::move(compare));
    }

    // Modifying
    template<typename TIterator, typename TValue>
    void Replace(TIterator first, TIterator last, const TValue& oldValue, const TValue& newValue)
    {
        std::replace(first, last, oldValue, newValue);
    }

    template<typename TIterator, typename TPredicate, typename TValue>
    void ReplaceIf(TIterator first, TIterator last, TPredicate predicate, const TValue& newValue)
    {
        std::replace_if(first, last, std::move(predicate), newValue);
    }

    template<typename TInputIterator, typename TOutputIterator>
    TOutputIterator Copy(TInputIterator first, TInputIterator last, TOutputIterator destination)
    {
        return std::copy(first, last, destination);
    }

    template<typename TInputIterator, typename TOutputIterator, typename TPredicate>
    TOutputIterator CopyIf(TInputIterator first, TInputIterator last, TOutputIterator destination, TPredicate predicate)
    {
        return std::copy_if(first, last, destination, std::move(predicate));
    }

    template<typename TInputIterator, typename TOutputIterator>
    TOutputIterator Move(TInputIterator first, TInputIterator last, TOutputIterator destination)
    {
        return std::move(first, last, destination);
    }

    template<typename TIterator>
    void Reverse(TIterator first, TIterator last)
    {
        std::reverse(first, last);
    }

    template<typename TIterator, typename TOutputIterator>
    TOutputIterator ReverseCopy(TIterator first, TIterator last, TOutputIterator destination)
    {
        return std::reverse_copy(first, last, destination);
    }

    template<typename TIterator>
    void Rotate(TIterator first, TIterator middle, TIterator last)
    {
        std::rotate(first, middle, last);
    }

    template<typename TIterator, typename TGenerator>
    void Generate(TIterator first, TIterator last, TGenerator generator)
    {
        std::generate(first, last, std::move(generator));
    }

    template<typename TIterator, typename TValue>
    void Fill(TIterator first, TIterator last, const TValue& value)
    {
        std::fill(first, last, value);
    }

    template<typename TIterator>
    void SwapRanges(TIterator first, TIterator last, TIterator otherFirst)
    {
        std::swap_ranges(first, last, otherFirst);
    }

    // Remove / Unique
    // Remove and RemoveIf do not resize containers; use the erase-remove idiom.
    template<typename TIterator, typename TValue>
    TIterator Remove(TIterator first, TIterator last, const TValue& value)
    {
        return std::remove(first, last, value);
    }

    template<typename TIterator, typename TPredicate>
    TIterator RemoveIf(TIterator first, TIterator last, TPredicate predicate)
    {
        return std::remove_if(first, last, std::move(predicate));
    }

    template<typename TIterator>
    TIterator Unique(TIterator first, TIterator last)
    {
        return std::unique(first, last);
    }

    template<typename TIterator, typename TBinaryPredicate>
    TIterator Unique(TIterator first, TIterator last, TBinaryPredicate predicate)
    {
        return std::unique(first, last, std::move(predicate));
    }


    // Numeric
    template<typename TIterator, typename TValue>
    [[nodiscard]] TValue Accumulate(TIterator first, TIterator last, TValue initialValue)
    {
        return std::accumulate(first, last, std::move(initialValue));
    }

    template<typename TIterator, typename TValue, typename TBinaryOperation>
    [[nodiscard]] TValue Accumulate(TIterator first, TIterator last, TValue initialValue, TBinaryOperation operation)
    {
        return std::accumulate(first, last, std::move(initialValue), std::move(operation));
    }

    template<typename TInputIterator, typename TOutputIterator>
    TOutputIterator PartialSum(TInputIterator first, TInputIterator last, TOutputIterator destination)
    {
        return std::partial_sum(first, last, destination);
    }

    template<typename TInputIterator, typename TOutputIterator, typename TBinaryOperation>
    TOutputIterator PartialSum(TInputIterator first, TInputIterator last, TOutputIterator destination, TBinaryOperation operation)
    {
        return std::partial_sum(first, last, destination, std::move(operation));
    }

    template<typename TInputIterator, typename TOutputIterator>
    TOutputIterator AdjacentDifference(TInputIterator first, TInputIterator last, TOutputIterator destination)
    {
        return std::adjacent_difference(first, last, destination);
    }

    template<typename TInputIterator, typename TOutputIterator, typename TBinaryOperation>
    TOutputIterator AdjacentDifference(TInputIterator first, TInputIterator last, TOutputIterator destination, TBinaryOperation operation)
    {
        return std::adjacent_difference(first, last, destination, std::move(operation));
    }


    // Iterator Utilities
    template<typename TIterator>
    [[nodiscard]] constexpr auto Distance(TIterator first, TIterator last)
    {
        return std::distance(first, last);
    }

    template<typename TIterator, typename TDistance>
    constexpr void Advance(TIterator& iterator, TDistance distance)
    {
        std::advance(iterator, distance);
    }

    template<typename TIterator, typename TDistance>
    [[nodiscard]] constexpr TIterator Next(TIterator iterator, TDistance distance = 1)
    {
        return std::next(iterator, distance);
    }

    template<typename TIterator, typename TDistance>
    [[nodiscard]] constexpr TIterator Prev(TIterator iterator, TDistance distance = 1)
    {
        return std::prev(iterator, distance);
    }
}