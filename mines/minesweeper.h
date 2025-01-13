#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <random>
#include <queue>

class Minesweeper {
private:
    int gamestatus_ = 0;
    std::time_t begintime_;
    std::time_t endtime_;
    std::vector<std::vector<int>> openfield_;
    std::vector<std::vector<int>> field_;
    size_t width_;
    size_t height_;

public:
    struct Cell {
        size_t x = 0;
        size_t y = 0;
    };

private:
    void OpenCells(Cell cell);
    void Defeat();
    void Victory();
    void CheckVictory();

public:
    enum class GameStatus {
        NOT_STARTED,
        IN_PROGRESS,
        VICTORY,
        DEFEAT,
    };

    using RenderedField = std::vector<std::string>;

    Minesweeper(size_t width, size_t height, size_t mines_count);
    Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void NewGame(size_t width, size_t height, size_t mines_count);
    void NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines);

    void OpenCell(const Cell& cell);
    void MarkCell(const Cell& cell);

    GameStatus GetGameStatus() const;
    time_t GetGameTime() const;

    RenderedField RenderField() const;
};
