// Этот модуль является посредником между конкретными классами фильтров и некими описаниями
// параметров объектов фильтров (filter descriptor) поэтому он знает всё и про первых, и про вторых.

#pragma once

#include "cmd_arg_parser.h"
#include "filters.h"
#include "filter_pipeline.h"
#include "map"

class FilterPipelineFactory {
public:
    using FilterFactory = BaseFilter* (*)(const FilterDescriptor&);  // Указатель на функцию
    using FilterToMakerMap = std::map<std::string_view, FilterFactory>;

public:
    void AddFilterMaker(std::string_view name, FilterFactory factory);

    // По заданному имени фильтра возвращает указатель на функцию (фактори)
    FilterFactory GetFilterFactoryMaker(std::string_view name) const;

    // Создаёт фильтр по переданному ему описанию параметров командной строки
    // пользуясь шестерёнками, добавленными ранее
    BaseFilter* CreateFilter(const FilterDescriptor& descriptor) const;

    bool CreateFilterPipeline(FilterPipeline& fp, const CmdLineParser::FilterDescriptorVector& fdv) const;


protected:
    FilterToMakerMap filter_to_makers_;
};

namespace FilterFactories {
    size_t SWtoSize(const std::string_view& sw); //перевод std::string_view в size_t

    // Тут мы фактически хардкодим список фильтров, доступных нашей программе.
    // Добавление нового фильтра осуществляется путём создания новой продуцирующей функции.
    BaseFilter* MakeGaussianBlurFilter(const FilterDescriptor& fd);

    BaseFilter* MakeCropFilter(const FilterDescriptor& fd);

    BaseFilter* MakeNegativeFilter(const FilterDescriptor& fd);

    BaseFilter* MakeGrayscaleFilter(const FilterDescriptor& fd);

    BaseFilter* MakeSharpeningFilter(const FilterDescriptor& fd);

    BaseFilter* MakeEdgeDetectionFilter(const FilterDescriptor& fd);

    BaseFilter* MakeLanczosScaleFilter(const FilterDescriptor& fd);

}



