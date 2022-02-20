/**
 * Copyright (C) 2022  Chistyakov Alexander
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

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

