#pragma once
#include "cmd_arg_parser.h"
#include "filter_pipeline_factory.h"
#include "bitmap.h"

class App {
public:
    void Setup();
    void Run(int argc, char* argv[]);
protected:
    CmdLineParser cmd_parser_;
    FilterPipelineFactory fpf_;
    FilterPipeline fp_;
    Bitmap bmp_;
};
