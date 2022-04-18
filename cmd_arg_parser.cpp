#include "cmd_arg_parser.h"

const char* CmdLineParser::MANUAL = "Description of the format of arguments in cmd:\n"
                                    "{program name} {input file path} {output file path} [-{filter name 1} [filter param 1] [filter param 2] ...]\n"
                                    "[-{filter name 2} [filter param 1] [filter param 2] ...] ...\n"
                                    "Args in {} are mandatory, args in [] are optional.\n"
                                    "List of all filters:\n"
                                    "Crop (-crop width height)\n"
                                    "Crops an image with given width and height. Top left side of an image is used.\n"
                                    "If given width and height are bigger than the size of an image, program returns an available part of given image.\n"
                                    "Grayscale (-gs)\n"
                                    "Converts the image to shades of gray.\n"
                                    "Negative (-neg)\n"
                                    "Converts the image to negative.\n"
                                    "Sharpening (-sharp)\n"
                                    "Self explanatory.\n"
                                    "Edge Detection (-edge threshold)\n"
                                    "Applies grayscale, sharpens, then pixels with a value greater than threshold are colored white, the rest are black.\n"
                                    "Gaussian Blur (-blur sigma)\n"
                                    "Gaussian Blur with sigma parameter.\n"
                                    "Lanczos Scale (-scale width height [alpha])\n"
                                    "Scales image to given width and height with alpha parameter. Default alpha value is 3.";


CmdLineParser::parse_result CmdLineParser::Parse(int argc, char* argv[]) {
    if (argc == ZERO_PARAM_NUM) {
        return CmdLineParser::parse_result::HELP;
    } else if (argc < MIN_PARAM_NUM) {
        return CmdLineParser::parse_result::FAILED; // Недостаточно параметров
    }
    input_file_name_ = argv[INPUT_FILE_NAME_POS];
    output_file_name_ = argv[OUTPUT_FILE_NAME_POS];
    if (argc == MIN_PARAM_NUM) {
        return CmdLineParser::parse_result::PARSED;
    }
    if (argc > MIN_PARAM_NUM && argv[OUTPUT_FILE_NAME_POS + 1][0] != '-') {
        return CmdLineParser::parse_result::FAILED;
    }
    FilterDescriptor struct_holder;
    std::string_view arg;
    for (int i = OUTPUT_FILE_NAME_POS + 1; i < argc; ++i) {
        arg = argv[i]; // перевел в string_view
        if (arg[0] == '-') {
            if (i != OUTPUT_FILE_NAME_POS + 1) { // чтобы не добавить пустую структуру
                fdv_.push_back(struct_holder);
            }
            struct_holder = FilterDescriptor(); // обнулил холдер структуры
            arg.remove_prefix(1);
            if (arg.empty()) {
                return CmdLineParser::parse_result::FAILED;
            }
            struct_holder.filter_name = arg;
        } else {
            struct_holder.filter_params.push_back(arg);
        }
    }
    fdv_.push_back(struct_holder);
    return CmdLineParser::parse_result::PARSED;
}