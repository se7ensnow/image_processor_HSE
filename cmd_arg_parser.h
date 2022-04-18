#pragma once

#include <string_view>
#include <vector>
#include <iostream>

struct FilterDescriptor {
    std::string_view filter_name;
    std::vector<std::string_view> filter_params;
};

class CmdLineParser {
public:
    static const int ZERO_PARAM_NUM = 1;
    static const int MIN_PARAM_NUM = 3;
    static const int INPUT_FILE_NAME_POS = 1;
    static const int OUTPUT_FILE_NAME_POS = 2;
    static const char* MANUAL;
    enum parse_result {
        HELP,
        PARSED,
        FAILED
    };
public:
    using FilterDescriptorVector = std::vector<FilterDescriptor>;
public:
    parse_result Parse(int argc, char* argv[]);  // enum для разных случаев пользовательского ввода
    std::string_view GetInputFileName() const { return input_file_name_; }
    std::string_view GetOutputFileName() const { return output_file_name_; }
    FilterDescriptorVector GetData() const { return fdv_; }

protected:
    std::string_view input_file_name_;
    std::string_view output_file_name_;
    FilterDescriptorVector fdv_;
};
