#include "minesweeper.h"

void Minesweeper::OpenCells(Cell cell) {
    openfield_[cell.x][cell.y] = 1;
    std::queue<Cell> q;
    q.push(cell);
    Cell curcell;
    while (!q.empty()) {
        curcell = q.front();
        if (field_[curcell.x][curcell.y] == 0) {
            for (int i = (curcell.x == 0 ? 0 : -1); i <= (curcell.x == width_ - 1 ? 0 : 1); ++i) {
                for (int j = (curcell.y == 0 ? 0 : -1); j <= (curcell.y == height_ - 1 ? 0 : 1); ++j) {
                    if (openfield_[curcell.x + i][curcell.y + j] == 0) {
                        openfield_[curcell.x + i][curcell.y + j] = 1;
                        q.push({curcell.x + i, curcell.y + j});
                    }
                }
            }
        }
        q.pop();
    }
}
void Minesweeper::Defeat() {
    gamestatus_ = 3;
    endtime_ = std::time(nullptr);
    for (size_t x = 0; x < width_; ++x) {
        for (size_t y = 0; y < height_; ++y) {
            openfield_[x][y] = 1;
        }
    }
}
void Minesweeper::Victory() {
    gamestatus_ = 2;
    endtime_ = std::time(nullptr);
}
void Minesweeper::CheckVictory() {
    bool check = true;
    for (size_t x = 0; x < width_; ++x) {
        for (size_t y = 0; y < height_; ++y) {
            if (field_[x][y] != -1 and openfield_[x][y] != 1) {
                check = false;
                break;
            }
        }
    }
    if (check) {
        Victory();
    }
}

Minesweeper::Minesweeper(size_t width, size_t height, size_t mines_count) {
    gamestatus_ = 0;
    openfield_.assign(width, std::vector<int>(height));
    field_.assign(width, std::vector<int>(height));
    width_ = width;
    height_ = height;
    std::vector<size_t> rangen;
    for (size_t i = 0; i < width * height; ++i) {
        rangen.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    g.seed(time(0));
    std::shuffle(rangen.begin(), rangen.end(), g);
    std::vector<Cell> cells_with_mines;
    for (size_t i = 0; i < mines_count; ++i) {
        cells_with_mines.push_back({(rangen[i] - rangen[i] % height) / height, rangen[i] % height});
    }
    for (auto [x, y] : cells_with_mines) {
        field_[x][y] = -1;
    }
    int c = 0;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (field_[x][y] != -1) {
                c = 0;
                for (int i = (x == 0 ? 0 : -1); i <= (x == width - 1 ? 0 : 1); ++i) {
                    for (int j = (y == 0 ? 0 : -1); j <= (y == height - 1 ? 0 : 1); ++j) {
                        if (field_[x + i][y + j] == -1) {
                            ++c;
                        }
                    }
                }
                field_[x][y] = c;
            }
        }
    }
}
Minesweeper::Minesweeper(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    gamestatus_ = 0;
    openfield_.assign(width, std::vector<int>(height));
    field_.assign(width, std::vector<int>(height));
    width_ = width;
    height_ = height;
    for (auto [x, y] : cells_with_mines) {
        field_[x][y] = -1;
    }
    int c = 0;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (field_[x][y] != -1) {
                c = 0;
                for (int i = (x == 0 ? 0 : -1); i <= (x == width - 1 ? 0 : 1); ++i) {
                    for (int j = (y == 0 ? 0 : -1); j <= (y == height - 1 ? 0 : 1); ++j) {
                        if (field_[x + i][y + j] == -1) {
                            ++c;
                        }
                    }
                }
                field_[x][y] = c;
            }
        }
    }
}

void Minesweeper::NewGame(size_t width, size_t height, size_t mines_count) {
    gamestatus_ = 0;
    openfield_.clear();
    field_.clear();
    openfield_.assign(width, std::vector<int>(height));
    field_.assign(width, std::vector<int>(height));
    width_ = width;
    height_ = height;
    std::vector<size_t> rangen;
    for (size_t i = 0; i < width * height; ++i) {
        rangen.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    g.seed(time(0));
    std::shuffle(rangen.begin(), rangen.end(), g);
    std::vector<Cell> cells_with_mines;
    for (size_t i = 0; i < mines_count; ++i) {
        cells_with_mines.push_back({(rangen[i] - rangen[i] % height) / height, rangen[i] % height});
    }
    for (auto [x, y] : cells_with_mines) {
        field_[x][y] = -1;
    }
    int c = 0;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (field_[x][y] != -1) {
                c = 0;
                for (int i = (x == 0 ? 0 : -1); i <= (x == width - 1 ? 0 : 1); ++i) {
                    for (int j = (y == 0 ? 0 : -1); j <= (y == height - 1 ? 0 : 1); ++j) {
                        if (field_[x + i][y + j] == -1) {
                            ++c;
                        }
                    }
                }
                field_[x][y] = c;
            }
        }
    }
}
void Minesweeper::NewGame(size_t width, size_t height, const std::vector<Cell>& cells_with_mines) {
    gamestatus_ = 0;
    openfield_.clear();
    field_.clear();
    openfield_.assign(width, std::vector<int>(height));
    field_.assign(width, std::vector<int>(height));
    width_ = width;
    height_ = height;
    for (auto [x, y] : cells_with_mines) {
        field_[x][y] = -1;
    }
    int c = 0;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            if (field_[x][y] != -1) {
                c = 0;
                for (int i = (x == 0 ? 0 : -1); i <= (x == width - 1 ? 0 : 1); ++i) {
                    for (int j = (y == 0 ? 0 : -1); j <= (y == height - 1 ? 0 : 1); ++j) {
                        if (field_[x + i][y + j] == -1) {
                            ++c;
                        }
                    }
                }
                field_[x][y] = c;
            }
        }
    }
}

void Minesweeper::OpenCell(const Cell& cell) {
    switch (gamestatus_) {
        case 0:
            begintime_ = std::time(nullptr);
            gamestatus_ = 1;
            break;
        case 2:
            return;
            break;
        case 3:
            return;
            break;
    }
    if (openfield_[cell.x][cell.y] == 2 or openfield_[cell.x][cell.y] == 1) {
        return;
    }
    if (field_[cell.x][cell.y] != -1) {
        OpenCells(cell);
        CheckVictory();
    } else {
        Defeat();
    }
}
void Minesweeper::MarkCell(const Cell& cell) {
    switch (gamestatus_) {
        case 0:
            begintime_ = std::time(nullptr);
            gamestatus_ = 1;
            break;
        case 2:
            return;
            break;
        case 3:
            return;
            break;
    }
    if (openfield_[cell.x][cell.y] == 0) {
        openfield_[cell.x][cell.y] = 2;
    } else if (openfield_[cell.x][cell.y] == 2) {
        openfield_[cell.x][cell.y] = 0;
    }
}

Minesweeper::GameStatus Minesweeper::GetGameStatus() const {
    return static_cast<GameStatus>(gamestatus_);
}
time_t Minesweeper::GetGameTime() const {
    switch (gamestatus_) {
        case 0:
            return static_cast<time_t>(0);
        case 1:
            return std::time(nullptr) - begintime_;
        default:
            return endtime_ - begintime_;
    }
}

Minesweeper::RenderedField Minesweeper::RenderField() const {
    RenderedField renderedfield;
    std::string cur;
    for (size_t y = 0; y < height_; ++y) {
        cur = "";
        for (size_t x = 0; x < width_; ++x) {
            if (openfield_[x][y] == 1) {
                if (field_[x][y] == -1) {
                    cur.push_back('*');
                } else if (field_[x][y] == 0) {
                    cur.push_back('.');
                } else {
                    cur += std::to_string(field_[x][y]);
                }
            } else if (openfield_[x][y] == 2) {
                cur.push_back('?');
            } else {
                cur.push_back('-');
            }
        }
        renderedfield.push_back(cur);
    }
    return renderedfield;
}