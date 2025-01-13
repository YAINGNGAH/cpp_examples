#pragma once

#include <string>

// Should not allow reuse and yell under sanitizers.
// Fix the interface and implementation.
// AwesomeCallback should add "awesomeness".

class OneTimeCallback {
public:
    OneTimeCallback() = default;
    explicit OneTimeCallback(const std::string& s);
    virtual ~OneTimeCallback() = default;

    virtual std::string operator()() const&&;

protected:
    std::string s_;
};

// Implement ctor, operator(), maybe something else...
class AwesomeCallback : public OneTimeCallback {
public:
    AwesomeCallback();
    explicit AwesomeCallback(const std::string& s);
    ~AwesomeCallback() = default;
    std::string operator()() const&& override;
};
