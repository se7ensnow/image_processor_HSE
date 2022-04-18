#include "app.h"


int main(int argc, char* argv[]) {
    try {
        App app;
        app.Setup();
        app.Run(argc, argv);
    } catch(std::exception& e) {
        std::cerr <<e.what() <<std::endl;
    }
}
