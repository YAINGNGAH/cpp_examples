#pragma once
#include "image.h"
#include "filter_template.h"

class Negative : Filter {
public:
    Negative() = default;
    void Use(Image& im) override;
};