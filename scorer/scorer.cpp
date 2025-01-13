#include "scorer.h"

ScoreTable GetScoredStudents(const Events& events, time_t score_time) {
    ScoreTable scoretable;
    std::map<StudentName, std::map<TaskName, std::pair<int64_t, int64_t>>> mergetable;
    std::map<StudentName, std::map<TaskName, std::pair<int64_t, bool>>> checktable;

    for (const Event& i : events) {
        if (i.time > score_time) {
            continue;
        }
        if (checktable.find(i.student_name) == checktable.end()) {
            checktable[i.student_name];
        }
        if (checktable[i.student_name].find(i.task_name) == checktable[i.student_name].end()) {
            checktable[i.student_name][i.task_name] = std::pair(0, false);
        }
        if (mergetable.find(i.student_name) == mergetable.end()) {
            mergetable[i.student_name];
        }
        if (mergetable[i.student_name].find(i.task_name) == mergetable[i.student_name].end()) {
            mergetable[i.student_name][i.task_name] = std::pair(-1, 0);
        }
        if (i.event_type == EventType::CheckFailed) {
            if (checktable[i.student_name][i.task_name].first <= i.time) {
                checktable[i.student_name][i.task_name].second = false;
                checktable[i.student_name][i.task_name].first = i.time;
            }
        } else if (i.event_type == EventType::CheckSuccess) {
            if (checktable[i.student_name][i.task_name].first <= i.time) {
                checktable[i.student_name][i.task_name].second = true;
                checktable[i.student_name][i.task_name].first = i.time;
            }
        } else if (i.event_type == EventType::MergeRequestOpen) {
            mergetable[i.student_name][i.task_name].first =
                std::max(mergetable[i.student_name][i.task_name].first, i.time);
        } else {
            mergetable[i.student_name][i.task_name].second =
                std::max(mergetable[i.student_name][i.task_name].second, i.time);
        }
    }
    for (auto& i : checktable) {
        for (auto& j : checktable[i.first]) {
            if (checktable[i.first][j.first].second &&
                mergetable[i.first][j.first].second > mergetable[i.first][j.first].first) {
                if (scoretable.find(i.first) == scoretable.end()) {
                    scoretable[i.first];
                }
                scoretable[i.first].insert(j.first);
            }
        }
    }
    return scoretable;
}
