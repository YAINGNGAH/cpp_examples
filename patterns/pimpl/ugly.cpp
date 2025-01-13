#include "ugly.h"
#include "bad.h"
#include <exception>

struct SplineImpl {
private:
    std::vector<double> x_;
    std::vector<double> y_;
    std::vector<double> y2_;

public:
    SplineImpl(const std::vector<double>& x, const std::vector<double>& y, double a, double b)
        : x_(x), y_(y) {
        y2_.resize(x_.size());
        int e = mySplineSnd(x_.data(), y_.data(), x_.size(), a, b, y2_.data());
        switch (e) {
            case -1:
                throw std::logic_error("Memory allocation error");
            case -2:
                throw std::logic_error("x is not monitonic increasing.");
        }
    }
    double Interpolate(double x) {
        double y;
        int e = mySplintCube(x_.data(), y_.data(), y2_.data(), x_.size(), x, &y);
        switch (e) {
            case -1:
                throw std::logic_error("Array length is 0");
            case -2:
                throw std::logic_error("X is not monitonic increasing");
        }
        return y;
    }
};

Spline::Spline(const std::vector<double>& x, const std::vector<double>& y, double a, double b) {
    impl_ = std::make_shared<SplineImpl>(x, y, a, b);
}
double Spline::Interpolate(double x) {
    return impl_->Interpolate(x);
}
