#pragma once
#include "filter_template.h"
#include "image.h"

const double KERLSIZE = 4.;
const double HALF = 0.5;

class Blur : Filter {
private:
    double sigma_;

public:
    explicit Blur(double sigma);
    void Use(Image& im) override;
};