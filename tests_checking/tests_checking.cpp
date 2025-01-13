#include "tests_checking.h"
#include "deque"

std::vector<std::string> StudentsOrder(const std::vector<StudentAction>& student_actions,
                                       const std::vector<size_t>& queries) {
    std::deque<std::string> works;
    std::vector<std::string> answer;
    for (auto const& i : student_actions) {
        if (i.side == Side::Top) {
            works.push_front(i.name);
        } else {
            works.push_back(i.name);
        }
    }
    for (auto const& i : queries) {
        answer.push_back(works[i - 1]);
    }
    return answer;
}
