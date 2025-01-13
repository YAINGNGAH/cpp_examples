#pragma once

#include "dungeon.h"

#include <stdexcept>
#include <queue>
#include <unordered_set>
#include <list>

Room* FindFinalRoom(Room* starting_room) {
    std::queue<Room*> q;
    q.push(starting_room);
    std::list<Door*> closed_doors;
    std::vector<std::string> keys = {""};
    std::unordered_set<Room*> used;
    while (!q.empty() && !q.front()->IsFinal()) {
        Room* room = q.front();
        used.insert(room);
        q.pop();
        for (size_t i = 0; i < room->NumKeys(); ++i) {
            keys.push_back(room->GetKey(i));
        }
        for (size_t i = 0; i < room->NumDoors(); ++i) {
            closed_doors.push_back(room->GetDoor(i));
        }
        auto door = closed_doors.begin();
        while (door != closed_doors.end()) {
            for (size_t j = 0; j < keys.size(); ++j) {
                (*door)->TryOpen(keys[j]);
            }
            if ((*door)->IsOpen()) {
                if (!used.contains((*door)->GoThrough())) {
                    q.push((*door)->GoThrough());
                }
                closed_doors.erase(door++);
            } else {
                ++door;
            }
        }
    }
    if (q.empty()) {
        return nullptr;
    }
    return q.front();
}
