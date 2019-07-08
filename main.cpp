#include "parser.h"

int main() {
    parser *p = new parser();

    while (true) {
        p->receive();
    }
    return 0;
}