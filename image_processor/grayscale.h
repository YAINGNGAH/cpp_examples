#pragma once
#include "image.h"
#include "filter_template.h"

class Grayscale : Filter {
public:
    Grayscale() = default;
    void Use(Image &im) override;
};