#pragma once
#include <memory>
#include <array>
#include <stack>

template <class T>
class ImmutableVector {
private:
    struct Node {
        std::array<std::shared_ptr<Node>, 32> child_;
        T value_;
        Node() {
            for (size_t i = 0; i < 32; ++i) {
                child_[i] = nullptr;
            }
        }
        Node(const Node& rhs) : child_(rhs.child_), value_(rhs.value_) {
        }
        Node(std::shared_ptr<Node> rhs) : child_((*rhs).child_), value_((*rhs).value_) {
        }
    };

    std::shared_ptr<Node> root_;
    size_t size_;

public:
    explicit ImmutableVector(std::shared_ptr<Node> root, size_t size) {
        root_ = root;
        size_ = size;
    }
    ImmutableVector() {
        root_ = std::make_shared<Node>();
        size_ = 0;
    }

    explicit ImmutableVector(size_t count, const T& value = T()) {
        root_ = std::make_shared<Node>();
        for (size_t i = 0; i < count; i += 32) {
            Node* n = root_.get();
            size_t bitmask = 31;
            size_t h = 0;
            while (bitmask < i) {
                bitmask <<= 5;
                h += 5;
                bitmask += 31;
            }
            while (bitmask > 0) {
                if (bitmask == 31) {
                    for (int j = 0; j < 32 && (i + j < count); ++j) {
                        n->child_[j] = std::make_shared<Node>();
                        n->child_[j]->value_ = value;
                    }
                }
                size_t j = (i & bitmask) >> h;
                h -= 5;
                n = n->child_[j].get();
                bitmask >>= 5;
            }
        }
        size_ = count;
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        root_ = std::make_shared<Node>();
        auto it = first;
        size_t count = 0;
        while (it != last) {
            count++;
            it++;
        }
        it = first;
        for (size_t i = 0; i < count; i += 32) {
            Node* n = root_.get();
            size_t bitmask = 31;
            size_t h = 0;
            while (bitmask < i) {
                bitmask <<= 5;
                h += 5;
                bitmask += 31;
            }
            while (bitmask > 0) {
                if (bitmask == 31) {
                    for (int j = 0; j < 32 && (i + j < count); ++j) {
                        n->child_[j] = std::make_shared<Node>();
                        n->child_[j]->value_ = *it;
                        it++;
                    }
                }
                size_t j = (i & bitmask) >> h;
                h -= 5;
                n = n->child_[j].get();
                bitmask >>= 5;
            }
        }
        size_ = count;
    }

    ImmutableVector(std::initializer_list<T> l) {
        root_ = std::make_shared<Node>();
        size_t count = l.size();
        auto it = l.begin();
        for (size_t i = 0; i < count; i += 32) {
            Node* n = root_.get();
            size_t bitmask = 31;
            size_t h = 0;
            while (bitmask < i) {
                bitmask <<= 5;
                h += 5;
                bitmask += 31;
            }
            while (bitmask > 0) {
                if (bitmask == 31) {
                    for (int j = 0; j < 32 && (i + j < count); ++j) {
                        n->child_[j] = std::make_shared<Node>();
                        n->child_[j]->value_ = *it;
                        it++;
                    }
                }
                size_t j = (i & bitmask) >> h;
                h -= 5;
                n = n->child_[j].get();
                bitmask >>= 5;
            }
        }
        size_ = count;
    }

    ImmutableVector Set(size_t index, const T& value) {
        std::shared_ptr<Node> newroot = std::make_shared<Node>(root_);
        Node* n = newroot.get();
        size_t bitmask = 31;
        size_t h = 0;
        while (bitmask < index) {
            bitmask <<= 5;
            h += 5;
            bitmask += 31;
        }
        while (bitmask > 0) {
            size_t i = (index & bitmask) >> h;
            h -= 5;
            std::shared_ptr<Node> c = std::make_shared<Node>(n->child_[i]);
            n->child_[i] = c;
            n = c.get();
            if (bitmask == 31) {
                n->value_ = value;
            }
            bitmask >>= 5;
        }
        return ImmutableVector<T>(newroot, size_);
    }

    const T& Get(size_t index) const {
        Node* n = root_.get();
        size_t bitmask = 31;
        size_t h = 0;
        while (bitmask < index) {
            bitmask <<= 5;
            bitmask += 31;
            h += 5;
        }
        while (bitmask > 0) {
            size_t i = (index & bitmask) >> h;
            h -= 5;
            n = n->child_[i].get();
            if (bitmask == 31) {
                return n->value_;
            }
            bitmask >>= 5;
        }
    }

    ImmutableVector PushBack(const T& value) {
        std::shared_ptr<Node> newroot = std::make_shared<Node>(root_);
        Node* n = newroot.get();
        size_t bitmask = 31;
        size_t h = 0;
        while (bitmask < Size()) {
            bitmask <<= 5;
            h += 5;
            bitmask += 31;
        }
        while (bitmask > 0) {
            size_t i = (Size() & bitmask) >> h;
            h -= 5;
            if (n->child_[i]) {
                std::shared_ptr<Node> c = std::make_shared<Node>(n->child_[i]);
                n->child_[i] = c;
                n = c.get();
                bitmask >>= 5;
            } else {
                n->child_[i] = std::make_shared<Node>();
                n->child_[i]->value_ = value;
                break;
            }
        }
        return ImmutableVector<T>(newroot, size_ + 1);
    }

    ImmutableVector PopBack() {
        std::shared_ptr<Node> newroot = std::make_shared<Node>(root_);
        Node* n = newroot.get();
        size_t bitmask = 31;
        size_t h = 0;
        while (bitmask < Size() - 1) {
            bitmask <<= 5;
            h += 5;
            bitmask += 31;
        }
        while (bitmask > 0) {
            size_t i = ((Size() - 1) & bitmask) >> h;
            h -= 5;
            if (bitmask == 31) {
                n->child_[i] = nullptr;
                return ImmutableVector<T>(newroot, size_ - 1);
            }
            std::shared_ptr<Node> c = std::make_shared<Node>(n->child_[i]);
            n->child_[i] = c;
            n = c.get();
            bitmask >>= 5;
        }
    }

    size_t Size() const {
        return size_;
    }
};
