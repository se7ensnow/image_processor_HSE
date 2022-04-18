#include "filter_pipeline_factory.h"

#include <stdexcept>

namespace FilterFactories {
    // TODO: Перенести из FilterFactories
    size_t SWtoSize(const std::string_view& sw) {
        std::string str_holder = std::string(sw);
        uint64_t uint_holder = std::stoul(str_holder);
        size_t result = size_t(uint_holder);
        return result;
    }

    BaseFilter* MakeGaussianBlurFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "blur") {
            throw std::invalid_argument("wrong blur filter descriptor");
        }
        if (fd.filter_params.size() != GaussianBlurFilter::PARAM_NUM) {
            throw std::invalid_argument("wrong blur filter params size");
        }
        double sigma;
        try {
            sigma = std::stod(std::string(fd.filter_params[0]));
        } catch (std::invalid_argument& e) {
            throw std::invalid_argument("wrong blur filter param type");
        }
        return new GaussianBlurFilter(sigma);
    }

    BaseFilter* MakeCropFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "crop") {
            throw std::invalid_argument("wrong crop filter descriptor");
        }
        if (fd.filter_params.size() != CropFilter::PARAM_NUM) {
            throw std::invalid_argument("wrong crop filter params size");
        }
        size_t width;
        size_t height;
        try {
            width = SWtoSize(fd.filter_params[0]);
        } catch(std::invalid_argument& e) {
            throw std::invalid_argument("wrong crop filter width param type");
        }
        try {
            height = SWtoSize(fd.filter_params[1]);
        } catch(std::invalid_argument& e) {
            throw std::invalid_argument("wrong crop filter height param type");
        }
        return new CropFilter(width, height);
    }

    BaseFilter* MakeNegativeFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "neg") {
            throw std::invalid_argument("wrong negative filter descriptor");
        }
        if (!fd.filter_params.empty()) {
            throw std::invalid_argument("wrong negative filter params size");
        }
        return new NegativeFilter;
    }

    BaseFilter* MakeGrayscaleFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "gs") {
            throw std::invalid_argument("wrong grayscale filter descriptor");
        }
        if (!fd.filter_params.empty()) {
            throw std::invalid_argument("wrong grayscale filter params size");
        }
        return new GrayscaleFilter;
    }

    BaseFilter* MakeSharpeningFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "sharp") {
            throw std::invalid_argument("wrong sharpening filter descriptor");
        }
        if (!fd.filter_params.empty()) {
            throw std::invalid_argument("wrong sharpening filter params size");
        }
        return new SharpeningFilter;
    }

    BaseFilter* MakeEdgeDetectionFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "edge") {
            throw std::invalid_argument("wrong edge detection filter descriptor");
        }
        if (fd.filter_params.size() != EdgeDetectionFilter::PARAM_NUM) {
            throw std::invalid_argument("wrong edge detection filter params size");
        }
        double threshold;
        try {
            threshold = std::stod(std::string(fd.filter_params[0]));
        } catch (std::invalid_argument& e) {
            throw std::invalid_argument("wrong edge detection filter param type");
        }
        return new EdgeDetectionFilter(threshold);
    }

    BaseFilter* MakeLanczosScaleFilter(const FilterDescriptor& fd) {
        if (fd.filter_name != "scale") {
            throw std::invalid_argument("wrong lanczos scale filter descriptor");
        }

        if (fd.filter_params.size() == LanczosScaleFilter::PARAM_NUM_WITH_ALPHA) {
            size_t width;
            size_t height;
            int alpha;
            try {
                width = SWtoSize(fd.filter_params[0]);
            } catch(std::invalid_argument& e) {
                throw std::invalid_argument("wrong lanczos filter width param type");
            }
            try {
                height = SWtoSize(fd.filter_params[1]);
            } catch(std::invalid_argument& e) {
                throw std::invalid_argument("wrong lanczos filter height param type");
            }
            try {
                alpha = std::stoi(std::string(fd.filter_params[2]));
            } catch (std::invalid_argument& e) {
                throw std::invalid_argument("wrong lanczos filter alpha param type");
            }
            return new LanczosScaleFilter(width, height, alpha);
        }

        if (fd.filter_params.size() == LanczosScaleFilter::PARAM_NUM_WO_ALPHA) {
            size_t width;
            size_t height;
            try {
                width = SWtoSize(fd.filter_params[0]);
            } catch(std::invalid_argument& e) {
                throw std::invalid_argument("wrong lanczos filter width param type");
            }
            try {
                height = SWtoSize(fd.filter_params[1]);
            } catch(std::invalid_argument& e) {
                throw std::invalid_argument("wrong lanczos filter height param type");
            }
            return new LanczosScaleFilter(width, height);
        } else {
            throw std::invalid_argument("wrong lanczos scale filter params size");
        }
    }
}

void FilterPipelineFactory::AddFilterMaker(std::string_view name, FilterFactory factory) {
    filter_to_makers_.insert({name, factory});
}

FilterPipelineFactory::FilterFactory FilterPipelineFactory::GetFilterFactoryMaker(std::string_view name) const {
    FilterToMakerMap::const_iterator it = filter_to_makers_.find(name);
    if (it != filter_to_makers_.end()) {
        return it->second;
    }
    return nullptr;
}

BaseFilter* FilterPipelineFactory::CreateFilter(const FilterDescriptor& descriptor) const {
    FilterFactory gear = GetFilterFactoryMaker(descriptor.filter_name);
    if (!gear) {
        throw std::invalid_argument(std::string(descriptor.filter_name) + " is not a valid filter name");
    }
    BaseFilter* current_filter = gear(descriptor);
    return current_filter;
}

bool FilterPipelineFactory::CreateFilterPipeline(FilterPipeline& fp, const CmdLineParser::FilterDescriptorVector& fdv) const {
    for (const FilterDescriptor& i : fdv) {
        BaseFilter* filter_template = nullptr;
        try {
            filter_template = CreateFilter(i);
        } catch(std::invalid_argument& e) {
            std::cerr << e.what() <<std::endl;
            return false;
        } catch(std::out_of_range& e) {
            std::cerr << e.what() <<std::endl;
            return false;
        }
        fp.AddFilter(filter_template);
    }
    return true;
}
