#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include <string>
#include <stdexcept>

// ----------------- Node -----------------
class ListNode {
public:
    std::string value;
    ListNode* prev;
    ListNode* next;

    ListNode(const std::string& val)
        : value(val), prev(nullptr), next(nullptr) {}
};

// ----------------- LinkedList -----------------
class LinkedList {
public:
    ListNode* head;
    ListNode* tail;
    size_t size;

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}
    ~LinkedList();

    // Push/Pop operations
    void push_front(const std::string& val);
    void push_back(const std::string& val);
    std::string pop_front();
    std::string pop_back();

    // Random access (O(n))
    std::string get(long long index) const;
    void set(long long index, const std::string& val);

    // Helpers
    bool empty() const { return size == 0; }
};

#endif // LINKEDLIST_HPP
