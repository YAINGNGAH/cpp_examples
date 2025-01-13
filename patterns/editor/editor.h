#pragma once
#include <string>
#include <vector>
#include <memory>

class Editor {
private:
    std::string txt_;
    size_t ind_;

    class Command {
    public:
        virtual void Do() = 0;
        virtual void Undo() = 0;
        virtual bool Done() {
            return true;
        }

        virtual ~Command() = default;
    };

    class TypeCommand : public Command {
    private:
        Editor& edit_;
        char c_;

    public:
        TypeCommand(Editor& edit, char c) : edit_(edit), c_(c) {
        }
        void Do() override {
            edit_.txt_.insert(edit_.ind_, 1, c_);
            edit_.ind_++;
        }
        void Undo() override {
            edit_.ind_--;
            edit_.txt_.erase(edit_.ind_, 1);
        }
    };

    class ShiftLeftCommand : public Command {
    private:
        Editor& edit_;
        bool done_;

    public:
        ShiftLeftCommand(Editor& edit) : edit_(edit) {
        }
        void Do() override {
            if (edit_.ind_ == 0) {
                done_ = false;
            } else {
                edit_.ind_--;
                done_ = true;
            }
        }
        bool Done() override {
            return done_;
        }
        void Undo() override {
            if (done_) {
                edit_.ind_++;
            }
        }
    };

    class ShiftRightCommand : public Command {
    private:
        Editor& edit_;
        bool done_;

    public:
        ShiftRightCommand(Editor& edit) : edit_(edit) {
        }
        void Do() override {
            if (edit_.ind_ >= edit_.txt_.size()) {
                done_ = false;
            } else {
                edit_.ind_++;
                done_ = true;
            }
        }
        bool Done() override {
            return done_;
        }
        void Undo() override {
            if (done_) {
                edit_.ind_--;
            }
        }
    };

    class BackspaceCommand : public Command {
    private:
        Editor& edit_;
        char c_;
        bool done_;

    public:
        BackspaceCommand(Editor& edit) : edit_(edit) {
        }
        void Do() override {
            if (edit_.ind_ == 0) {
                done_ = false;
            } else {
                edit_.ind_--;
                c_ = edit_.txt_[edit_.ind_];
                done_ = true;
                edit_.txt_.erase(edit_.ind_, 1);
            }
        }
        bool Done() override {
            return done_;
        }
        void Undo() override {
            if (done_) {
                edit_.txt_.insert(edit_.ind_, 1, c_);
                edit_.ind_++;
            }
        }
    };

    std::vector<std::unique_ptr<Command>> commands_;
    size_t lastcommand_;
    void Clear() {
        if (lastcommand_ + 1 < commands_.size()) {
            commands_.erase(commands_.begin() + lastcommand_ + 1, commands_.end());
        }
    }

public:
    Editor() : txt_() {
        ind_ = 0;
        lastcommand_ = std::string::npos;
    }
    const std::string& GetText() const {
        return txt_;
    }

    void Type(char c) {
        Clear();
        std::unique_ptr<Command> command = std::unique_ptr<Command>(new TypeCommand(*this, c));
        commands_.push_back(std::move(command));
        commands_.back()->Do();
        lastcommand_ = commands_.size() - 1;
    }

    void ShiftLeft() {
        Clear();
        std::unique_ptr<Command> command = std::unique_ptr<Command>(new ShiftLeftCommand(*this));
        commands_.push_back(std::move(command));
        commands_.back()->Do();
        if (!commands_.back()->Done()) {
            commands_.pop_back();
            return;
        }
        lastcommand_ = commands_.size() - 1;
    }

    void ShiftRight() {
        Clear();
        std::unique_ptr<Command> command = std::unique_ptr<Command>(new ShiftRightCommand(*this));
        commands_.push_back(std::move(command));
        commands_.back()->Do();
        if (!commands_.back()->Done()) {
            commands_.pop_back();
            return;
        }
        lastcommand_ = commands_.size() - 1;
    }

    void Backspace() {
        Clear();
        std::unique_ptr<Command> command = std::unique_ptr<Command>(new BackspaceCommand(*this));
        commands_.push_back(std::move(command));
        commands_.back()->Do();
        if (!commands_.back()->Done()) {
            commands_.pop_back();
            return;
        }
        lastcommand_ = commands_.size() - 1;
    }

    void Undo() {
        if (lastcommand_ == std::string::npos) {
            return;
        }
        commands_[lastcommand_]->Undo();
        lastcommand_--;
    }

    void Redo() {
        if (lastcommand_ + 1 >= commands_.size()) {
            return;
        }
        lastcommand_++;
        commands_[lastcommand_]->Do();
    }
};
