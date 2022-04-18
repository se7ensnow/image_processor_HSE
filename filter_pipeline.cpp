#include "filter_pipeline.h"

void FilterPipeline::Apply(Bitmap& image) {
    for (BaseFilter* i : fv_) {
        i->Apply(image);
    }
}

void FilterPipeline::AddFilter(BaseFilter* new_filter) {
    fv_.push_back(new_filter);
}

FilterPipeline::~FilterPipeline() {
    for (BaseFilter* i : fv_) {
        delete i;
    }
}
