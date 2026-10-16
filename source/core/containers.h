#pragma once
#include <array>
#include <bitset>
#include <deque>
#include <forward_list>
#include <initializer_list>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace Khan
{
	// Sequence Containers
	template<typename T, std::size_t N>
	using TInlineArray = std::array<T, N>;

	template<typename T, typename TAllocator = std::allocator<T>>
	using TArray = std::vector<T, TAllocator>;

	template<typename T, typename TAllocator = std::allocator<T>>
	using TList = std::list<T, TAllocator>;

	template<typename T, typename TAllocator = std::allocator<T>>
	using TForwardList = std::forward_list<T, TAllocator>;

	template<typename T, typename TAllocator = std::allocator<T>>
	using TDeque = std::deque<T, TAllocator>;

	// Ordered Associative Containers
	template<typename TKey, typename TValue, typename TCompare = std::less<TKey>, typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
	using TMap = std::map<TKey, TValue, TCompare, TAllocator>;

	template<typename TKey, typename TValue, typename TCompare = std::less<TKey>, typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
	using TMultiMap = std::multimap<TKey, TValue, TCompare, TAllocator>;

	template<typename T, typename TCompare = std::less<T>, typename TAllocator = std::allocator<T>>
	using TSet = std::set<T, TCompare, TAllocator>;

	template<typename T, typename TCompare = std::less<T>, typename TAllocator = std::allocator<T>>
	using TMultiSet = std::multiset<T, TCompare, TAllocator>;

	// Unordered Associative Containers
	template<typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TKeyEqual = std::equal_to<TKey>, typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
	using THashMap = std::unordered_map<TKey, TValue, THash, TKeyEqual, TAllocator>;

	template<typename TKey, typename TValue, typename THash = std::hash<TKey>, typename TKeyEqual = std::equal_to<TKey>, typename TAllocator = std::allocator<std::pair<const TKey, TValue>>>
	using THashMultiMap = std::unordered_multimap<TKey, TValue, THash, TKeyEqual, TAllocator>;

	template<typename T, typename THash = std::hash<T>, typename TKeyEqual = std::equal_to<T>, typename TAllocator = std::allocator<T>>
	using THashSet = std::unordered_set<T, THash, TKeyEqual, TAllocator>;

	template<typename T, typename THash = std::hash<T>, typename TKeyEqual = std::equal_to<T>, typename TAllocator = std::allocator<T>>
	using THashMultiSet = std::unordered_multiset<T, THash, TKeyEqual, TAllocator>;

	// Container Adaptors
	template<typename T, typename TContainer = std::deque<T>>
	using TStack = std::stack<T, TContainer>;

	template<typename T, typename TContainer = std::deque<T>>
	using TQueue = std::queue<T, TContainer>;

	template<typename T, typename TContainer = std::vector<T>, typename TCompare = std::less<typename TContainer::value_type>>
	using TPriorityQueue = std::priority_queue<T, TContainer, TCompare>;

	// Fixed-Size Bit Containers
	template<std::size_t N>
	using BitSet = std::bitset<N>;

	// Dynamic Bit Containers
	using BitArray = std::vector<bool>;

	// Initializer Lists
	template<typename T>
	using TInitializerList = std::initializer_list<T>;
}