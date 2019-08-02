#include <iostream>

int main() {
    bool run = true;
    while (run) {
        char c = std::cin.get();
        std::string s;
        switch (c) {
        case 'p':
            std::cin >> s;
            while (s != "p") {
                std::cout << s << std::endl;
                std::cin >> s;
            }
            break;
        case 'k':
            std::cout << "no" << std::endl;
            break;
        case 'h':
            std::cout << "we" << std::endl;
            break;
        case 'j':
            std::cout << "so" << std::endl;
            break;
        case 'l':
            std::cout << "ea" << std::endl;
            break;
        default:
            std::cout << c;
        }
    }
}
