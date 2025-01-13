#include "admission.h"
#include "algorithm"
#include "tuple"

bool operator==(const Date& a, const Date& b) {
    if (std::tie(a.day, a.month, a.year) == std::tie(b.day, b.month, b.year)) {
        return true;
    }
    return false;
}

bool operator<(const Date& a, const Date& b) {
    if (a == b) {
        return false;
    }
    if (std::tie(a.year, a.month, a.day) > std::tie(b.year, b.month, b.day)) {  // Перегрузил специально под компаратор
        return true;
    }
    return false;
}

AdmissionTable FillUniversities(const std::vector<University>& universities, const std::vector<Applicant>& applicants) {
    AdmissionTable admissiontable;
    std::vector<const Applicant*> stud_rate;
    for (const auto& i : applicants) {
        stud_rate.push_back(&i);
    }
    std::sort(stud_rate.begin(), stud_rate.end(), [](const Applicant* a, const Applicant* b) -> bool {
        if (std::tie(a->points, a->student.birth_date, a->student.name) <
            std::tie(b->points, b->student.birth_date, b->student.name)) {
            return false;
        }
        return true;
    });
    std::unordered_map<std::string, size_t> un_num;
    for (auto const& i : universities) {
        un_num[i.name] = i.max_students;
    }
    for (auto const& i : stud_rate) {
        for (auto const& j : i->wish_list) {
            if (admissiontable.find(j) == admissiontable.end()) {
                admissiontable[j] = std::vector<const Student*>();
            }
            if (admissiontable[j].size() < un_num[j]) {
                admissiontable[j].push_back(&(i->student));
                break;
            }
        }
    }
    return admissiontable;
}
