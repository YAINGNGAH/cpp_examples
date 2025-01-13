#pragma once

#include <algorithm>

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const;

    void Unlink();

    ~ListHook();  // check

    ListHook(const ListHook&) = delete;

private:
    ListHook* before_ = nullptr;
    ListHook* next_ = nullptr;

    template <class T>
    friend class List;

    // that helper function might be useful
    void LinkBefore(ListHook* other);
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    private:
        ListHook* p_;
        friend class List<T>;
        Iterator(ListHook* p);

    public:
        Iterator& operator++();
        Iterator operator++(int);

        T& operator*() const;
        T* operator->() const;

        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;
    };

    List();
    List(const List&) = delete;
    List(List&& other);  // check

    // must unlink all elements from list
    ~List();  // check

    List& operator=(const List&) = delete;
    List& operator=(List&& other);  // check

    bool IsEmpty() const;
    // that method is allowed to be O(n)
    size_t Size() const;

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem);
    void PushFront(T* elem);

    T& Front();  // check
    const T& Front() const;

    T& Back();  // check
    const T& Back() const;

    void PopBack();
    void PopFront();

    Iterator Begin();
    Iterator End();

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element);

private:
    ListHook dummy_;
};

ListHook::~ListHook() {
    Unlink();
}

bool ListHook::IsLinked() const {
    if (before_ == nullptr && next_ == nullptr) {
        return false;
    }
    return true;
}

void ListHook::Unlink() {
    if (before_ != nullptr && next_ != nullptr) {
        next_->before_ = before_;
        before_->next_ = next_;
    }
    before_ = nullptr;
    next_ = nullptr;
}

void ListHook::LinkBefore(ListHook* other) {
    if (other->before_ != nullptr) {
        other->before_->next_ = this;
        before_ = other->before_;
    }
    if (next_ != nullptr) {
        other->next_ = next_;
        next_->before_ = other;
    }
    other->before_ = this;
    next_ = other;
}

template <typename T>
List<T>::List() {
    dummy_ = ListHook();
    dummy_.next_ = &dummy_;
    dummy_.before_ = &dummy_;
}

template <typename T>
List<T>::List(List<T>&& other) {
    dummy_ = other.dummy_;
    other.dummy_.next_ = &other.dummy_;
    other.dummy_.before_ = &other.dummy_;
}

template <typename T>
List<T>::~List() {
    ListHook* i = &dummy_;
    while (i != nullptr) {
        ListHook* next = i->next_;
        i->before_ = nullptr;
        i->next_ = nullptr;
        i = next;
    }
}

template <typename T>
List<T>& List<T>::operator=(List<T>&& other) {
    dummy_ = other.dummy_;
    other.dummy_.next_ = &other.dummy_;
    other.dummy_.before_ = &other.dummy_;
    return *this;
}

template <typename T>
bool List<T>::IsEmpty() const {
    if (dummy_.next_ == &dummy_ && dummy_.before_ == &dummy_) {
        return true;
    }
    return false;
}

template <typename T>
size_t List<T>::Size() const {
    size_t size = 0;
    for (ListHook* i = dummy_.next_; i != &dummy_; i = i->next_) {
        ++size;
    }
    return size;
}

template <typename T>
void List<T>::PushBack(T* elem) {
    reinterpret_cast<ListHook*>(elem)->LinkBefore(&dummy_);
}

template <typename T>
void List<T>::PushFront(T* elem) {
    dummy_.LinkBefore(reinterpret_cast<ListHook*>(elem));
}

template <typename T>  // maybe fix later
const T& List<T>::Front() const {
    return *reinterpret_cast<const T*>(dummy_.next_);
}

template <typename T>
T& List<T>::Front() {
    return *reinterpret_cast<T*>(dummy_.next_);
}

template <typename T>  // maybe fix later
const T& List<T>::Back() const {
    return *reinterpret_cast<const T*>(dummy_.before_);
}

template <typename T>
T& List<T>::Back() {
    return *reinterpret_cast<T*>(dummy_.before_);
}

template <typename T>
void List<T>::PopBack() {
    dummy_.before_->Unlink();
}

template <typename T>
void List<T>::PopFront() {
    dummy_.next_->Unlink();
}

template <typename T>
List<T>::Iterator::Iterator(ListHook* p) {
    p_ = p;
}

template <typename T>
typename List<T>::Iterator& List<T>::Iterator::operator++() {
    p_ = p_->next_;
    return *this;
}

template <typename T>
typename List<T>::Iterator List<T>::Iterator::operator++(int) {
    p_ = p_->next_;
    return Iterator(p_->before_);
}

template <typename T>
bool List<T>::Iterator::operator!=(const List::Iterator& rhs) const {
    if (p_ != rhs.p_) {
        return true;
    }
    return false;
}

template <typename T>
T& List<T>::Iterator::operator*() const {
    return *reinterpret_cast<T*>(p_);
}

template <typename T>
T* List<T>::Iterator::operator->() const {
    return reinterpret_cast<T*>(p_);
}

template <typename T>
bool List<T>::Iterator::operator==(const List::Iterator& rhs) const {
    if (p_ == rhs.p_) {
        return true;
    }
    return false;
}
// dnt knw what I am doing need a fix maybe
template <typename T>
typename List<T>::Iterator List<T>::Begin() {
    return Iterator(dummy_.next_);
}

template <typename T>
typename List<T>::Iterator List<T>::End() {
    return Iterator(&dummy_);
}

template <typename T>
typename List<T>::Iterator List<T>::IteratorTo(T* element) {
    return Iterator(reinterpret_cast<ListHook*>(element));
}

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
