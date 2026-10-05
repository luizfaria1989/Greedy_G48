#include "app/CommandLineApp.hpp"

int main(int argc, char** argv) {
    app::CommandLineApp app(argc, argv);
    return app.run();
}
