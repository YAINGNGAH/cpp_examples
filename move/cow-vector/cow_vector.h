#pragma once

#include <cstdint>
#include <string>
#include <vector>

class State {
private:
    int ref_count_;
    std::vector<std::string> v_;

public:
    State() {
        ref_count_ = 0;
    }
    explicit State(std::vector<std::string>& v);
    std::vector<std::string>& Data();
    ~State();
    size_t Size() const;
    void NewCopy();
    void DelCopy();
    int Copies();
};

class COWVector {
public:
    COWVector();
    ~COWVector();

    COWVector(const COWVector& other);
    COWVector& operator=(const COWVector& other);

    COWVector(COWVector&& other);
    COWVector& operator=(COWVector&& other);

    size_t Size() const;

    void Resize(size_t size);

    const std::string& Get(size_t at);
    const std::string& Back();

    void PushBack(const std::string& value);

    void Set(size_t at, const std::string& value);

private:
    State* state_;
};
