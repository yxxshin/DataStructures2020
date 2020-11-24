#ifndef _DEQUE_H
#define _DEQUE_H

#include <string>
#include <iostream>
#include <type_traits>
#include <optional>
#include <iostream>
#include <memory>
#include <cassert>

/* NOTE: Deque, ArrayDeque, ListDeque Declaration modification is not allowed.
 * Fill in the TODO sections in the following code. */
template <typename T>
class Deque {
public:
    virtual ~Deque() = default;

    /* NOTE: We won't implement push functions that take rvalue references. */
    virtual void push_front(const T&) = 0;
    virtual void push_back(const T&) = 0;

    /* NOTE: Unlike STL implementations which have separate `front` and
       pop_front` functions, we have one unified method for removing an elem. */
    virtual std::optional<T> remove_front() = 0;
    virtual std::optional<T> remove_back() = 0;

    virtual bool empty() = 0;
    virtual size_t size() = 0;

    virtual T& operator[](size_t) = 0;
};

template <typename T>
class ArrayDeque : public Deque<T> {
public:
    ArrayDeque();
    ~ArrayDeque() = default;

    void push_front(const T&) override;
    void push_back(const T&) override;

    std::optional<T> remove_front() override;
    std::optional<T> remove_back() override;

    bool empty() override;
    size_t size() override;
    size_t capacity();

    T& operator[](size_t) override;

private:
    std::unique_ptr<T[]> arr;
    size_t front;
    size_t back;
    size_t size_;
    size_t capacity_;

    void resize();
};

template <typename T>
ArrayDeque<T>::ArrayDeque() :
    front{63 /* You can change this */},
    back{0 /* You can change this */},
    size_{0}, capacity_{64} {
    arr = std::make_unique<T[]>(capacity_);
}

template <typename T>
void ArrayDeque<T>::push_front(const T& item) {
	// TODO
	size_++;
	arr[front] = item;
    if(front == 0) front = capacity_;
    front--;   

    if(size_ == capacity_ - 2)
        resize();
}

template <typename T>
void ArrayDeque<T>::push_back(const T& item) {
    // TODO
    size_++;
    arr[back] = item;
    back++;
	if(back == capacity_) back = 0;

    if(size_ == capacity_ - 2)
        resize();
}

template <typename T>
std::optional<T> ArrayDeque<T>::remove_front() {
    // TODO
	if( !empty() ){
        size_--;
		if(front == capacity_-1) {
            front = 0;
            return arr[front];
        }
        front++;
		return arr[front];
	}
    return std::nullopt;
}

template <typename T>
std::optional<T> ArrayDeque<T>::remove_back() {
    // TODO
    if ( !empty() ){
        size_--;
        if(back == 0){
            back = capacity_-1;
            return arr[back];
        }
        back--;
        return arr[back];
    }
    return std::nullopt;
}

template <typename T>
void ArrayDeque<T>::resize() {
    // TODO
    int capacity_before = capacity_;
    capacity_ *= 2;

    // make a same array brr as arr (before resizing)
    std::unique_ptr<T[]> brr = std::make_unique<T[]>(capacity_before);
    for(int i = 0; i < capacity_before; i++){
        brr[i] = arr[i];
    }

    arr = std::make_unique<T[]>(capacity_);

    if(front > back){
        for(int i = front + 1; i < capacity_before; i++)
            arr[capacity_before + i] = brr[i];
        
        for(int i = 0; i < back; i++)
            arr[i] = brr[i];

        front += capacity_before;
    }

    else if(front < back){
        for(int i = front + 1; i <= back - 1; i++)
            arr[i] = brr[i];
    }

}

template <typename T>
bool ArrayDeque<T>::empty() {
    // TODO
    if(size_ == 0) return true;
    else return false;
}

template <typename T>
size_t ArrayDeque<T>::size() {
    // TODO
    return size_;
}

template <typename T>
size_t ArrayDeque<T>::capacity() {
    // TODO
    return capacity_;
}

template <typename T>
T& ArrayDeque<T>::operator[](size_t idx) {
    // TODO
    if(front < back){
        return arr[front + idx + 1];
    }
    else{
        if(idx >= capacity_ - front - 1){
            return arr[idx - capacity_ + front + 1];
        }
        else {
            return arr[front + idx + 1];
        }
    }
}

template<typename T>
struct ListNode {
    std::optional<T> value;
    ListNode* prev;
    ListNode* next;

    ListNode() : value(std::nullopt), prev(this), next(this) {}
    ListNode(const T& t) : value(t), prev(this), next(this) {}

    ListNode(const ListNode&) = delete;
};

template<typename T>
class ListDeque : public Deque<T> {
public:
    ListDeque();
    ~ListDeque();

    void push_front(const T&) override;
    void push_back(const T&) override;

    std::optional<T> remove_front() override;
    std::optional<T> remove_back() override;

    bool empty() override;
    size_t size() override;

    T& operator[](size_t) override;

    size_t size_ = 0;
    ListNode<T>* sentinel = nullptr;
};

template<typename T>
ListDeque<T>::ListDeque() : sentinel(new ListNode<T>{}), size_(0) {}

template<typename T>
void ListDeque<T>::push_front(const T& t) {
    // TODO
    size_++;
    ListNode<T>* insert_node = new ListNode(t);

    if(size_ == 1){
        sentinel->prev = insert_node;
        sentinel->next = insert_node;
        insert_node->prev = sentinel;
        insert_node->next = sentinel;
    }
    
    else {
        ListNode<T>* tmp = sentinel->next;
        sentinel->next = insert_node;
        insert_node->next = tmp;
        insert_node->prev = sentinel;
        tmp->prev = insert_node;   
    }
}

template<typename T>
void ListDeque<T>::push_back(const T& t) {
    // TODO
    size_++;
    ListNode<T>* insert_node = new ListNode(t);

    if(size_ == 1){
        sentinel->prev = insert_node;
        sentinel->next = insert_node;
        insert_node->prev = sentinel;
        insert_node->next = sentinel;
    }
    
    else {
        ListNode<T>* tmp = sentinel->prev;
        sentinel->prev = insert_node;
        insert_node->next = sentinel;
        insert_node->prev = tmp;
        tmp->next = insert_node;
    }
    
}

template<typename T>
std::optional<T> ListDeque<T>::remove_front() {
    // TODO
    if(size_ >= 1) {
        size_--;
        ListNode<T>* first_node = sentinel->next;
        ListNode<T>* second_node = first_node->next;
        sentinel->next = second_node;
        second_node->prev = sentinel;
        std::optional<T> val = first_node->value.value();
        delete first_node;
        return val;
    }
    else return std::nullopt;
}

template<typename T>
std::optional<T> ListDeque<T>::remove_back() {
    // TODO
    if(size_ >= 1) {
        size_--;
        ListNode<T>* first_node = sentinel->prev;
        ListNode<T>* second_node = first_node->prev;
        sentinel->prev = second_node;
        second_node->next = sentinel;
        std::optional<T> val = first_node->value.value();
        delete first_node;
        return val;
    }
    else return std::nullopt;
}

template<typename T>
bool ListDeque<T>::empty() {
    // TODO
    if(size_ == 0) return true;
    return false;
}

template<typename T>
size_t ListDeque<T>::size() {
    // TODO
    return size_;
}

template<typename T>
T& ListDeque<T>::operator[](size_t idx) {
    // TODO
    auto temp = sentinel;
    idx++;
    while( idx-- ){
        temp = temp->next;
    }
    return temp->value.value();
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ListNode<T>& n) {
    if (n.value)
        os << n.value.value();

    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const ListDeque<T>& l) {
    auto np = l.sentinel->next;
    while (np != l.sentinel) {
        os << *np << ' ';
        np = np->next;
    }

    return os;
}

template<typename T>
ListDeque<T>::~ListDeque() {
    // TODO
    if(!empty()){
        ListNode<T>* deleting_node = sentinel->next;
        while(deleting_node != NULL){
            ListNode<T>* next_node = deleting_node->next;
            delete deleting_node;
            if(next_node == sentinel) break;
            deleting_node = next_node;
        }
    }
    delete sentinel;
}

#endif // _DEQUE_H
