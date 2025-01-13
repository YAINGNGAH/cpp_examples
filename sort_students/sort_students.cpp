#include "sort_students.h"
#include "algorithm"
#include "cstring"
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
    if (std::tie(a.year, a.month, a.day) < std::tie(b.year, b.month, b.day)) {
        return true;
    }
    return false;
}

bool date_cmp(const Student& a, const Student& b) {
    return std::tie(a.birth_date, a.last_name, a.name) < std::tie(b.birth_date, b.last_name, b.name);
}

bool name_cmp(const Student& a, const Student& b) {
    return std::tie(a.last_name, a.name, a.birth_date) < std::tie(b.last_name, b.name, b.birth_date);
}

void SortStudents(std::vector<Student>& students, SortKind sortKind) {
    if (sortKind == SortKind::Name) {
        std::sort(students.begin(), students.end(), name_cmp);
    } else {
        std::sort(students.begin(), students.end(), date_cmp);
    }
}
