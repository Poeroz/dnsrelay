#include "../lib/parser.h"

int main() {
    std::cout << "Now : " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;

    parser *p = new parser();

    while (true) {
        p->receive();
    }
    return 0;
}