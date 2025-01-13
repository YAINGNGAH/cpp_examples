#include "scheme.h"




std::string Interpreter::Run(const std::string& s) {
    std::stringstream ss{s};
    Tokenizer tokenizer(&ss);
    std::shared_ptr<Object> root = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Incorrect Syntax");
    }
    SetScope(root, global_);
    if (root) {
        root = root->Eval();
        if (root){
            return root->Serialization();
        }else{
            return "()";
        }
    } else {
        throw RuntimeError("Nullptr");
    }
}
