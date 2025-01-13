#include "cow_vector.h"

State::State(std::vector<std::string> &v) {
    ref_count_ = 0;
    v_ = v;
}

State::~State() {
    v_.clear();
    ref_count_ = 0;
}

size_t State::Size() const {
    return v_.size();
}

void State::NewCopy() {
    ++ref_count_;
}

void State::DelCopy() {
    if (--ref_count_ == 0) {
        delete this;
    }
}

int State::Copies() {
    return ref_count_;
}

std::vector<std::string> &State::Data() {
    return v_;
}

COWVector::COWVector() {
    state_ = new State();
    state_->NewCopy();
}

COWVector::~COWVector() {
    state_->DelCopy();
    state_ = nullptr;
}

COWVector::COWVector(const COWVector &other) {
    state_ = other.state_;
    state_->NewCopy();
}

COWVector &COWVector::operator=(const COWVector &other) {
    if (&other == this) {
        return *this;
    }
    state_->DelCopy();
    state_ = other.state_;
    state_->NewCopy();
    return *this;
}

COWVector::COWVector(COWVector &&other) {
    state_ = other.state_;
    other.state_ = nullptr;
}
COWVector &COWVector::operator=(COWVector &&other) {
    if (&other == this) {
        return *this;
    }
    state_ = other.state_;
    other.state_ = nullptr;
    return *this;
}

size_t COWVector::Size() const {
    return state_->Size();
}

void COWVector::Resize(size_t size) {
    if (size == state_->Size()) {
        return;
    }
    if (state_->Copies() == 1) {
        state_->Data().resize(size);
        return;
    }
    std::vector<std::string> tmp = state_->Data();
    state_->DelCopy();
    tmp.resize(size);
    state_ = new State(tmp);
    state_->NewCopy();
}

const std::string &COWVector::Get(size_t at) {
    return state_->Data()[at];
}

const std::string &COWVector::Back() {
    return state_->Data().back();
}

void COWVector::PushBack(const std::string &value) {
    if (state_->Copies() == 1) {
        state_->Data().push_back(value);
        return;
    }
    std::vector<std::string> tmp = state_->Data();
    state_->DelCopy();
    tmp.push_back(value);
    state_ = new State(tmp);
    state_->NewCopy();
}

void COWVector::Set(size_t at, const std::string &value) {
    if (state_->Data()[at] == value) {
        return;
    }
    if (state_->Copies() == 1) {
        state_->Data()[at] = value;
        return;
    }
    std::vector<std::string> tmp = state_->Data();
    state_->DelCopy();
    tmp[at] = value;
    state_ = new State(tmp);
    state_->NewCopy();
}
