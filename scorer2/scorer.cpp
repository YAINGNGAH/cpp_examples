#include "scorer.h"

void Scorer::OnCheckFailed(const StudentName& student_name, const TaskName& task_name) {
    if (checktable_.find(student_name) != checktable_.end()) {
        checktable_[student_name].erase(task_name);
        if (checktable_[student_name].empty() and mergetable_[student_name].empty()) {
            checktable_.erase(student_name);
            mergetable_.erase(student_name);
        }
    }
}
void Scorer::OnCheckSuccess(const StudentName& student_name, const TaskName& task_name) {
    checktable_[student_name][task_name] = true;
}
void Scorer::OnMergeRequestOpen(const StudentName& student_name, const TaskName& task_name) {
    mergetable_[student_name][task_name] = true;
}
void Scorer::OnMergeRequestClosed(const StudentName& student_name, const TaskName& task_name) {
    mergetable_[student_name].erase(task_name);
}
void Scorer::Reset() {
    mergetable_.clear();
    checktable_.clear();
}
ScoreTable Scorer::GetScoreTable() const {
    ScoreTable scoretable;
    for (auto& i : checktable_) {
        for (auto& j : checktable_.at(i.first)) {
            if (mergetable_.count(i.first)) {
                if (!mergetable_.at(i.first).count(j.first) and j.second) {
                    if (!scoretable.count(i.first)) {
                        scoretable[i.first];
                    }
                    scoretable[i.first].insert(j.first);
                }
            } else {
                if (j.second) {
                    if (!scoretable.count(i.first)) {
                        scoretable[i.first];
                    }
                    scoretable[i.first].insert(j.first);
                }
            }
        }
    }
    return scoretable;
}