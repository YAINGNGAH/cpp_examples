#pragma once
#include <memory>
#include <string>
#include <vector>
#include <map>
class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

class ProducerBase {
public:
    virtual ~ProducerBase() = default;
    virtual std::unique_ptr<AbstractTest> Produce() = 0;
};

template <typename T>
class Producer : public ProducerBase {
public:
    std::unique_ptr<AbstractTest> Produce() override {
        return std::make_unique<T>();
    }
};

class TestRegistry {
private:
    std::map<std::string, std::unique_ptr<ProducerBase>> tests_;

public:
    static TestRegistry& Instance() {
        static TestRegistry test_registry;
        return test_registry;
    }
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        std::unique_ptr<ProducerBase> producer = std::make_unique<Producer<TestClass>>();
        tests_.insert({class_name, std::move(producer)});
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        return tests_.at(class_name)->Produce();
    }

    void RunTest(const std::string& test_name) {
        std::unique_ptr<AbstractTest> test = CreateTest(test_name);
        test->SetUp();
        try {
            test->Run();
        } catch (...) {
            test->TearDown();
            throw;
        }
        test->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> tests;
        for (auto i = tests_.begin(); i != tests_.end(); ++i) {
            if (callback(i->first)) {
                tests.push_back(i->first);
            }
        }
        return tests;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> tests;
        for (auto i = tests_.begin(); i != tests_.end(); ++i) {
            tests.push_back(i->first);
        }
        return tests;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        std::vector<std::string> tests;
        for (auto i = tests_.begin(); i != tests_.end(); ++i) {
            if (callback(i->first)) {
                RunTest(i->first);
            }
        }
    }

    void Clear() {
        tests_.clear();
    }
};

class FullMatch {
private:
    std::string s_;

public:
    FullMatch(const std::string& s) : s_(s) {
    }
    bool operator()(const std::string& s) {
        return s_ == s;
    }
};

class Substr {
private:
    std::string s_;

public:
    Substr(const std::string& s) : s_(s) {
    }
    bool operator()(const std::string& s) {
        return s.find(s_) != std::string::npos;
    }
};
