#include <chrono>
#include <iostream>
#include <thread>

#include "engine/life_engine.h"

void print(const life::engine::grid_t& grid)
{
    std::cout << std::endl;
    for (const life::engine::row_t& row : grid) {
        for (size_t c = 0; c < row.size(); ++c) {
            std::cout << (row[c] ? '*' : '-');
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main(int /*argc*/, char** /*argv[]*/)
{
    std::vector<std::vector<int>> begin = {
        {0, 0, 1, 0},
        {0, 0, 0, 1},
        {0, 1, 1, 1}
    };

    life::engine gl(20, 40);
    gl.start(begin, 1);

    size_t steps = 20;
    print(gl.grid());
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    while (--steps > 0) {
        gl.next_step();
        print(gl.grid());
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

