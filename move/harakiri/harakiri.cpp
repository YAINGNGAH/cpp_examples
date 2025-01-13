#include "harakiri.h"

OneTimeCallback::OneTimeCallback(const std::string &s) {
    s_ = s;
}

std::string OneTimeCallback::operator()() const && {
    std::string s = std::move(s_);
    delete this;
    return s;
}
AwesomeCallback::AwesomeCallback() : OneTimeCallback() {
}

AwesomeCallback::AwesomeCallback(const std::string &s) : OneTimeCallback(s) {
}

std::string AwesomeCallback::operator()() const && {
    std::string s = std::move(s_);
    delete this;
    return s + "awesomeness";
}
