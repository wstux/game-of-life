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

#ifndef LIFE_ENGINE_H
#define LIFE_ENGINE_H

#include <vector>

namespace life {

/**
 * \brief   Engine for Conway's Game of Life.
 */
class engine final
{
public:
    using row_t = std::vector<bool>;
    using grid_t = std::vector<row_t>;

    engine(const size_t row_count = 25, const size_t col_count = 25);

    bool next_step();

    bool restart(const grid_t& begin_state);

    bool start(const grid_t& begin_state);

    template<typename TType>
    bool start(const std::vector<std::vector<TType>>& begin, const TType& alive_val)
    {
        grid_t begin_state;
        begin_state.resize(m_row_count);
        for (row_t& r : begin_state) {
            r.resize(m_col_count, false);
        }

        for (size_t r = 0; r < begin.size(); ++r) {
            const std::vector<TType>& row = begin[r];
            for (size_t c = 0; c < row.size(); ++c) {
                begin_state[r][c] = (begin[r][c] == alive_val);
            }
        }

        return start(begin_state);
    }

    const grid_t& grid() const { return m_grid; }

    void stop();

private:
    size_t neighbors_count(const size_t row, const size_t col) const;

private:
    size_t m_row_count;
    size_t m_col_count;

    grid_t m_grid;
};

} // namespace life

#endif // LIFE_ENGINE_H

