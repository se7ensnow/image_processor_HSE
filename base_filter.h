#pragma once
#include "bitmap.h"

class BaseFilter {
public:
    virtual ~BaseFilter() = default;
    virtual void Apply(Bitmap& image) = 0;
};