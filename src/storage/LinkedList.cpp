
#include "storage/liststore.hpp"

// ----------------- Destructor -----------------
LinkedList::~LinkedList() {
    ListNode* current = head;
    while (current) {
        ListNode* next = current->next;
        delete current;
        current = next;
    }
}

// ----------------- Push Front -----------------
void LinkedList::push_front(const std::string& val) {
    ListNode* node = new ListNode(val);
    if (!head) {
        head = tail = node;
    } else {
        node->next = head;
        head->prev = node;
        head = node;
    }
    ++size;
}

// ----------------- Push Back -----------------
void LinkedList::push_back(const std::string& val) {
    ListNode* node = new ListNode(val);
    if (!tail) {
        head = tail = node;
    } else {
        node->prev = tail;
        tail->next = node;
        tail = node;
    }
    ++size;
}

// ----------------- Pop Front -----------------
std::string LinkedList::pop_front() {
    if (!head) throw std::runtime_error("list empty");

    ListNode* node = head;
    std::string val = node->value;

    head = head->next;
    if (head) head->prev = nullptr;
    else tail = nullptr;

    delete node;
    --size;
    return val;
}

// ----------------- Pop Back -----------------
std::string LinkedList::pop_back() {
    if (!tail) throw std::runtime_error("list empty");

    ListNode* node = tail;
    std::string val = node->value;

    tail = tail->prev;
    if (tail) tail->next = nullptr;
    else head = nullptr;

    delete node;
    --size;
    return val;
}

// ----------------- Get by index -----------------
std::string LinkedList::get(long long index) const {
    if (index < 0) index += size; // support negative indices
    if (index < 0 || index >= (long long)size)
        throw std::out_of_range("index out of range");

    ListNode* current = head;
    for (long long i = 0; i < index; ++i) current = current->next;
    return current->value;
}

// ----------------- Set by index -----------------
void LinkedList::set(long long index, const std::string& val) {
    if (index < 0) index += size;
    if (index < 0 || index >= (long long)size)
        throw std::out_of_range("index out of range");

    ListNode* current = head;
    for (long long i = 0; i < index; ++i) current = current->next;
    current->value = val;
}

// ================= MERGE SORT FOR LINKED LIST =================

static ListNode* split(ListNode* head) {
    ListNode* fast = head;
    ListNode* slow = head;

    while (fast->next && fast->next->next) {
        fast = fast->next->next;
        slow = slow->next;
    }

    ListNode* second = slow->next;
    slow->next = nullptr;
    if (second) second->prev = nullptr;
    return second;
}

static ListNode* merge(ListNode* a, ListNode* b, bool asc) {
    if (!a) return b;
    if (!b) return a;

    long long av = std::stoll(a->value);
    long long bv = std::stoll(b->value);

    bool takeA = asc ? (av <= bv) : (av >= bv);

    if (takeA) {
        a->next = merge(a->next, b, asc);
        if (a->next) a->next->prev = a;
        a->prev = nullptr;
        return a;
    } else {
        b->next = merge(a, b->next, asc);
        if (b->next) b->next->prev = b;
        b->prev = nullptr;
        return b;
    }
}

static ListNode* mergeSort(ListNode* head, bool asc) {
    if (!head || !head->next) return head;

    ListNode* second = split(head);

    head = mergeSort(head, asc);
    second = mergeSort(second, asc);

    return merge(head, second, asc);
}


void LinkedList::sort(bool ascending) {
    if (!head || size < 2) return;

    head = mergeSort(head, ascending);

    // Fix tail pointer
    tail = head;
    while (tail && tail->next)
        tail = tail->next;
}


// ---------- Clone (deep copy) ----------
LinkedList* LinkedList::clone() const {
    LinkedList* copy = new LinkedList();
    ListNode* curr = head;
    while (curr) {
        copy->push_back(curr->value);
        curr = curr->next;
    }
    return copy;
}