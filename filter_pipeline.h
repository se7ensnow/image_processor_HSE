#pragma once
#include <vector>
#include "base_filter.h"

class FilterPipeline {
public:
    using FilterVector = std::vector<BaseFilter*>;

public:
    ~FilterPipeline();

    void AddFilter(BaseFilter* new_filter);

    void Apply(Bitmap& image);

protected:
    FilterVector fv_;
};
