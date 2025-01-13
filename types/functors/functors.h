#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
private:
    Functor functor_;

public:
    ReverseBinaryFunctor(Functor functor) : functor_(functor) {
    }
    template <class T>
    bool operator()(T a, T b) {
        return functor_(b, a);
    }
};

template <class Functor>
class ReverseUnaryFunctor {
private:
    Functor functor_;

public:
    ReverseUnaryFunctor(Functor functor) : functor_(functor) {
    }
    template <class T>
    bool operator()(T x) {
        return !functor_(x);
    }
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}
