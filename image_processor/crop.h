#pragma once
#include "image.h"
#include "filter_template.h"

class Crop : Filter {
private:
    size_t w_;
    size_t h_;

public:
    explicit Crop(size_t w, size_t h);
    void Use(Image& im) override;
};