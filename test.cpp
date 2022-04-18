#include <catch.hpp>
#include "cmd_arg_parser.h"
#include "filter_pipeline_factory.h"
#include "filter_pipeline.h"
#include "filters.h"
#include "bitmap.h"
#include <stdexcept>

TEST_CASE("TestCmdLineParser") {
    CmdLineParser cmd;
    char exe_path[9] = "exe_path";
    int argc_zero = 1;
    char* argv_zero[1] = {exe_path};
    CmdLineParser::parse_result manual = cmd.Parse(argc_zero, argv_zero);
    REQUIRE(CmdLineParser::parse_result::HELP == manual);

    char file_input[24] = "../examples/example.bmp";
    int argc_not_enough = 2;
    char* argv_not_enough[2] = {exe_path, file_input};
    CmdLineParser::parse_result false_not_enough = cmd.Parse(argc_not_enough, argv_not_enough);
    REQUIRE(CmdLineParser::parse_result::FAILED == false_not_enough);

    char file_output[28] = "../examples/test_result.bmp";
    int argc_min = 3;
    char* argv_min[3] = {exe_path, file_input, file_output};
    CmdLineParser::parse_result true_min = cmd.Parse(argc_min, argv_min);
    REQUIRE(CmdLineParser::parse_result::PARSED == true_min);

    char wrong_filter_name[4] = "abc";
    int argc_wrong_filter_name = 4;
    char* argv_wrong_filter_name[4] = {exe_path, file_input, file_output,
                                       wrong_filter_name};
    CmdLineParser::parse_result false_wrong_filter_name = cmd.Parse(argc_wrong_filter_name, argv_wrong_filter_name);
    REQUIRE(CmdLineParser::parse_result::FAILED == false_wrong_filter_name);

    char empty_filter_name[2] = "-";
    int argc_empty_filter_name = 4;
    char* argv_empty_filter_name[4] = {exe_path, file_input, file_output,
                                       empty_filter_name};
    CmdLineParser::parse_result false_empty_filter_name = cmd.Parse(argc_empty_filter_name, argv_empty_filter_name);
    REQUIRE(CmdLineParser::parse_result::FAILED == false_empty_filter_name);

    char good_filter_name[6] = "-crop";
    char first_param[4] = "123";
    char second_param[4] = "456";
    int argc_parsed = 6;
    char* argv_parsed[6] = {exe_path, file_input, file_output,
                            good_filter_name, first_param, second_param};
    CmdLineParser::parse_result true_parsed = cmd.Parse(argc_parsed, argv_parsed);
    REQUIRE(CmdLineParser::parse_result::PARSED == true_parsed);

}

TEST_CASE("TestFilterFactories") {
    FilterDescriptor crop_wrong_name{"not_crop", {"123", "456"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeCropFilter(crop_wrong_name), "wrong crop filter descriptor");
    FilterDescriptor crop_wrong_size{"crop", {"456"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeCropFilter(crop_wrong_size), "wrong crop filter params size");
    FilterDescriptor crop_wrong_width_param{"crop", {"abc", "456"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeCropFilter(crop_wrong_width_param), "wrong crop filter width param type");
    FilterDescriptor crop_wrong_height_param{"crop", {"123", "abc"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeCropFilter(crop_wrong_height_param), "wrong crop filter height param type");

    FilterDescriptor neg_wrong_name{"not_neg", {}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeNegativeFilter(neg_wrong_name), "wrong negative filter descriptor");
    FilterDescriptor neg_wrong_size{"neg", {"123"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeNegativeFilter(neg_wrong_size), "wrong negative filter params size");

    FilterDescriptor gs_wrong_name{"not_gs", {}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeGrayscaleFilter(gs_wrong_name), "wrong grayscale filter descriptor");
    FilterDescriptor gs_wrong_size{"gs", {"123"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeGrayscaleFilter(gs_wrong_size), "wrong grayscale filter params size");

    FilterDescriptor sharp_wrong_name{"not_sharp", {}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeSharpeningFilter(sharp_wrong_name), "wrong sharpening filter descriptor");
    FilterDescriptor sharp_wrong_size{"sharp", {"123"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeSharpeningFilter(sharp_wrong_size), "wrong sharpening filter params size");

    FilterDescriptor edge_wrong_name{"not_edge", {"0.1"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeEdgeDetectionFilter(edge_wrong_name), "wrong edge detection filter descriptor");
    FilterDescriptor edge_wrong_size{"edge", {"0.1", "0.2"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeEdgeDetectionFilter(edge_wrong_size), "wrong edge detection filter params size");
    FilterDescriptor edge_wrong_params{"edge", {"abc"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeEdgeDetectionFilter(edge_wrong_params), "wrong edge detection filter param type");

    FilterDescriptor blur_wrong_name{"not_blur", {"3"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeGaussianBlurFilter(blur_wrong_name), "wrong blur filter descriptor");
    FilterDescriptor blur_wrong_size{"blur", {"3", "123"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeGaussianBlurFilter(blur_wrong_size), "wrong blur filter params size");
    FilterDescriptor blur_wrong_params{"blur", {"abc"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeGaussianBlurFilter(blur_wrong_params), "wrong blur filter param type");

    FilterDescriptor scale_wrong_name{"not_scale", {"123", "456", "3"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeLanczosScaleFilter(scale_wrong_name), "wrong lanczos scale filter descriptor");
    FilterDescriptor scale_wrong_size{"scale", {"123", "456", "11", "789"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeLanczosScaleFilter(scale_wrong_size), "wrong lanczos scale filter params size");
    FilterDescriptor scale_wrong_width_param{"scale", {"abc", "456"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeLanczosScaleFilter(scale_wrong_width_param), "wrong lanczos filter width param type");
    FilterDescriptor scale_wrong_height_param{"scale", {"123", "abc"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeLanczosScaleFilter(scale_wrong_height_param), "wrong lanczos filter height param type");
    FilterDescriptor scale_wrong_alpha_param{"scale", {"123", "456", "abs"}};
    REQUIRE_THROWS_WITH(FilterFactories::MakeLanczosScaleFilter(scale_wrong_alpha_param), "wrong lanczos filter alpha param type");
}

TEST_CASE("TestFilterPipelineFactory") {
    FilterPipelineFactory fpf;
    fpf.AddFilterMaker("blur", &FilterFactories::MakeGaussianBlurFilter);
    FilterPipelineFactory::FilterFactory gear = fpf.GetFilterFactoryMaker("blur");
    REQUIRE(gear);
    FilterPipelineFactory::FilterFactory non_existing_gear = fpf.GetFilterFactoryMaker("alpha");
    REQUIRE_FALSE(non_existing_gear);
    FilterDescriptor desc1{"blur", {"12"}};
    BaseFilter* filter = fpf.CreateFilter(desc1);
    REQUIRE(filter);
    delete filter;
}

TEST_CASE("TestBitmap") {
    Bitmap bmp_file;
    bool loaded_false = bmp_file.Load("../examples/non_existing_file.bmp");
    REQUIRE_FALSE(loaded_false);
    bool loaded_true = bmp_file.Load("../examples/example.bmp");
    REQUIRE(loaded_true);
    PixelArray image = bmp_file.GetPixels();
    REQUIRE(2048 == image.GetHeight());
    REQUIRE(2048 == image.GetWidth());
    bool created_true = bmp_file.CreateFile("../examples/test_result.bmp");
    REQUIRE(created_true);
}

TEST_CASE("TestFilters") {
    Bitmap bmp_file;
    bmp_file.Load("../examples/example.bmp");
    REQUIRE_NOTHROW(GaussianBlurFilter(2).Apply(bmp_file));
    REQUIRE_NOTHROW(CropFilter(1001, 1002).Apply(bmp_file));
    REQUIRE(bmp_file.GetPixels().GetWidth() == 1001);
    REQUIRE(bmp_file.GetPixels().GetHeight() == 1002);
    REQUIRE_NOTHROW(NegativeFilter().Apply(bmp_file));
    REQUIRE_NOTHROW(GrayscaleFilter().Apply(bmp_file));
    REQUIRE_NOTHROW(SharpeningFilter().Apply(bmp_file));
    REQUIRE_NOTHROW(EdgeDetectionFilter(0.05).Apply(bmp_file));
    REQUIRE_NOTHROW(LanczosScaleFilter(2001, 2002, 3).Apply(bmp_file));
    REQUIRE(bmp_file.GetPixels().GetWidth() == 2001);
    REQUIRE(bmp_file.GetPixels().GetHeight() == 2002);
}