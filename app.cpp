#include "app.h"


void App::Setup() {
    fpf_.AddFilterMaker("blur", &FilterFactories::MakeGaussianBlurFilter);
    fpf_.AddFilterMaker("crop", &FilterFactories::MakeCropFilter);
    fpf_.AddFilterMaker("neg", &FilterFactories::MakeNegativeFilter);
    fpf_.AddFilterMaker("gs", &FilterFactories::MakeGrayscaleFilter);
    fpf_.AddFilterMaker("sharp", &FilterFactories::MakeSharpeningFilter);
    fpf_.AddFilterMaker("edge", &FilterFactories::MakeEdgeDetectionFilter);
    fpf_.AddFilterMaker("scale", &FilterFactories::MakeLanczosScaleFilter);
}


void App::Run(int argc, char* argv[]) {
    CmdLineParser::parse_result cmd_parsed = cmd_parser_.Parse(argc, argv);
    if (cmd_parsed == CmdLineParser::parse_result::HELP) {
        std::cout <<CmdLineParser::MANUAL <<std::endl;
        return;
    }
    if (cmd_parsed == CmdLineParser::parse_result::FAILED) {
        std::cerr <<"Wrong program arguments" <<std::endl;
        return;
    }
    bool fp_created = fpf_.CreateFilterPipeline(fp_, cmd_parser_.GetData());
    if (!fp_created) {
        return;
    }
    std::string input_filename = std::string(cmd_parser_.GetInputFileName());
    if (!input_filename.ends_with(".bmp")) {
        std::cerr << "given input file is not bitmap";
        return;
    }
    std::string output_filename = std::string(cmd_parser_.GetOutputFileName());
    if (!output_filename.ends_with(".bmp")) {
        std::cerr << "given output file is not bitmap";
        return;
    }
    bool file_loaded = bmp_.Load(input_filename.c_str());
    if (!file_loaded) {
        std::cerr << "program cannot read the file" <<std::endl;
        return;
    }
    fp_.Apply(bmp_);
    bool file_writen = bmp_.CreateFile(output_filename.c_str());
    if (!file_writen) {
        std::cerr << "program cannot write the file" <<std::endl;
        return;
    }

}
