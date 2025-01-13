#pragma once
#include "image.h"

class Filter {
public:
    Filter() = default;
    virtual ~Filter() = default;
    virtual void Use(Image& im) = 0;
};
