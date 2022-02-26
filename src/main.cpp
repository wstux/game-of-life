/*
 * The MIT License
 *
 * Copyright 2022 Chistyakov Alexander.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "engine/life_engine.h"
#include "prog_opts/prog_opts.h"

namespace {

void split(const std::string& s, std::vector<std::string>& out_str, const std::string& delimiter)
{
    size_t begin_pos = 0;
    size_t pos = 0;
    while ((pos = s.find(delimiter, begin_pos)) != std::string::npos) {
        out_str.emplace_back(s.substr(begin_pos, pos - begin_pos));
        begin_pos = pos + delimiter.length();
    }
    if (pos == std::string::npos) {
        out_str.emplace_back(s.substr(begin_pos, s.size() - begin_pos));
    }
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter)
{
    std::vector<std::string> out_str;
    split(s, out_str, delimiter);
    return out_str;
}

life::engine::grid_t grid_form_stream(std::istream& in, const std::string& alive_state, const std::string& delimiter)
{
    life::engine::grid_t grid;

    size_t col_size = 0;
    std::string in_str;
    while(std::getline(in, in_str)) {
        const std::vector<std::string> r = split(in_str, delimiter);
        if (r.size() == 0) {
            return life::engine::grid_t();
        }
        if (col_size == 0) {
            col_size = r.size();
        }
        if (col_size != r.size()) {
            return life::engine::grid_t();
        }

        grid.emplace_back(life::engine::row_t(col_size));
        life::engine::row_t& row = grid.back();
        for (size_t i = 0; i < col_size; ++i) {
            row[i] = (r[i] == alive_state);
        }
    }
    return grid;
}

life::engine::grid_t grid_form_file(const std::string& file, const std::string& alive_state, const std::string& delimiter)
{
    std::ifstream in(file);
    if (! in.is_open()) {
        return life::engine::grid_t();
    }
    return grid_form_stream(in, alive_state, delimiter);
}

void print_grid(const life::engine::grid_t& grid)
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

} // <anonymous> namespace

int main(int argc, char* argv[])
{
    po::prog_opts po;
    po.insert<int>("-r,--row", 20, "Rows count. (default 20)");
    po.insert<int>("-c,--column", 40, "Columns count. (default 40)");
    po.insert<int>("-s,--step", 20, "Steps count. (default 20)");
    po.insert<std::string>("-a,--alive-state", "*", "Alive state. (default '*')");
    po.insert<std::string>("-d,--delimiter", " ", "Base state delimiter. (default ' ')");
    po.insert<std::string>("-f,--file", "Input file with base state.");
    po.insert("-h,--help", false, "Print this message.");

    if (po.has_error()) {
        std::cerr << po.error_msg() << std::endl;
        std::cout << po.usage() << std::endl;
        return EXIT_FAILURE;
    }

    if (! po.parse(argc, argv)) {
        std::cerr << po.error_msg() << std::endl;
        std::cout << po.usage() << std::endl;
        return EXIT_FAILURE;
    }

    if (po.value<bool>("--help")) {
        std::cout << po.usage() << std::endl;
        return EXIT_SUCCESS;
    }

    if (! po.has_value("--file")) {
        std::cerr << "Key '--file' is requared" << std::endl;
        std::cout << po.usage() << std::endl;
        return EXIT_FAILURE;
    }

    const size_t rows_count = po.value<int>("--row");
    const size_t cols_count = po.value<int>("--column");
    size_t step_count = po.value<int>("--step");

    const life::engine::grid_t begin_state = grid_form_file(po.value<std::string>("--file"),
                                                            po.value<std::string>("--alive-state"),
                                                            po.value<std::string>("--delimiter"));

    life::engine gl(rows_count, cols_count);
    gl.start(begin_state);

    do {
        print_grid(gl.grid());
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        gl.next_step();
    } while (--step_count > 0);

    return EXIT_SUCCESS;
}

