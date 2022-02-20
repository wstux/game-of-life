#include <sstream>
#include <vector>

#include "engine/life_engine.h"

#include "testdefs.h"

namespace {

using test_row_t = std::vector<int>;
using test_grid_t = std::vector<test_row_t>;

bool compare_grids(const test_grid_t& et, const life::engine::grid_t& grid)
{
    if (et.size() != grid.size()) {
        return false;
    }

    for (size_t r = 0; r < et.size(); ++r) {
        const test_row_t& et_row = et[r];
        const life::engine::row_t& row = grid[r];
        if (et_row.size() != row.size()) {
            return false;
        }

        for (size_t c = 0; c < et_row.size(); ++c) {
            const bool is_et_alive = (et_row[c] == 1);
            if (is_et_alive != row[c]) {
                return false;
            }
        }
    }
    return true;
}

std::string print_grid(const life::engine::grid_t& grid)
{
    std::stringstream ss;
    for (size_t r = 0; r < grid.size(); ++r) {
        const life::engine::row_t& row = grid[r];
        for (size_t c = 0; c < row.size(); ++c) {
            ss << (row[c] ? '*' : '-');
        }
        if (r != (grid.size() - 1)) {
            ss << std::endl;
        }
    }
    return ss.str();
}

} // <anonymous> namespace

TEST(life_engine, base)
{
    const std::vector<test_grid_t> etalon = {
        {{0, 1, 0, 0},
         {0, 0, 1, 0},
         {1, 1, 1, 0},
         {0, 0, 0, 0}},
        {{0, 0, 0, 0},
         {1, 0, 1, 0},
         {0, 1, 1, 0},
         {0, 1, 0, 0}},
        {{0, 0, 0, 0},
         {0, 0, 1, 0},
         {1, 0, 1, 0},
         {0, 1, 1, 0}},
        {{0, 0, 0, 0},
         {0, 1, 0, 0},
         {0, 0, 1, 1},
         {0, 1, 1, 0}},
        {{0, 0, 0, 0},
         {0, 0, 1, 0},
         {0, 0, 0, 1},
         {0, 1, 1, 1}}
    };

    test_grid_t begin = {{0, 1, 0, 0},
                         {0, 0, 1, 0},
                         {1, 1, 1, 0},
                         {0, 0, 0, 0}};

    life::engine gl(4, 4);
    gl.start(begin, 1);

    EXPECTED(compare_grids(etalon[0], gl.grid())) << "fail 0 step; grid state:" << std::endl
            << print_grid(gl.grid()) << std::endl;

    gl.next_step();
    EXPECTED(compare_grids(etalon[1], gl.grid())) << "fail 1 step; grid state:" << std::endl
            << print_grid(gl.grid()) << std::endl;

    gl.next_step();
    EXPECTED(compare_grids(etalon[2], gl.grid())) << "fail 2 step; grid state:" << std::endl
            << print_grid(gl.grid()) << std::endl;

    gl.next_step();
    EXPECTED(compare_grids(etalon[3], gl.grid())) << "fail 3 step; grid state:" << std::endl
            << print_grid(gl.grid()) << std::endl;

    gl.next_step();
    EXPECTED(compare_grids(etalon[4], gl.grid())) << "fail 4 step; grid state:" << std::endl
            << print_grid(gl.grid()) << std::endl;
}

int main()
{
    return RUN_TESTS();
}

