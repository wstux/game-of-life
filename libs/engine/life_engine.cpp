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

#include <algorithm>

#include "engine/life_engine.h"

namespace life {

engine::engine(const size_t row_count, const size_t col_count)
    : m_row_count(row_count)
    , m_col_count(col_count)
{}

size_t engine::neighbors_count(const size_t row, const size_t col) const
{
    using point_t = std::pair<int, int>;

    const int r = static_cast<int>(row);
    const int c = static_cast<int>(col);
    const std::vector<point_t> ps = {{r - 1, c - 1}, {r - 1, c}, {r - 1, c + 1},
                                     {r, c - 1},     /*{r, c},*/     {r, c + 1},
                                     {r + 1, c - 1}, {r + 1, c}, {r + 1, c + 1}};

    return std::count_if(ps.cbegin(), ps.cend(), [this](const point_t& p) -> bool {
                                                     return (p.first >= 0) && (p.first < (int)m_row_count) &&
                                                            (p.second >= 0) && (p.second < (int)m_col_count) &&
                                                            (m_grid[p.first][p.second]);
                                                 });
}

bool engine::next_step()
{
    grid_t next_step(m_row_count);
    std::for_each(next_step.begin(), next_step.end(), [this](row_t& r) { r.resize(m_col_count, false); });

    for (size_t r = 0; r < m_grid.size(); ++r) {
        const row_t& row = m_grid[r];
        for (size_t c = 0; c < row.size(); ++c) {
            const size_t n_count = neighbors_count(r, c);

			/*if (n_count < 2) {
				next_step[r][c] = false;
			} else if (n_count > 3) {
				next_step[r][c] = false;
			} else if (n_count == 3) {
				next_step[r][c] = true;
			} else if (m_grid[r][c] && (n_count == 2)) {
				next_step[r][c] = true;
			} else {
				next_step[r][c] = false;
			}*/

			if ((n_count == 3) || (m_grid[r][c] && (n_count == 2))) {
				next_step[r][c] = true;
			}
        }
    }

    std::swap(next_step, m_grid);
    return true;
}

bool engine::restart(const grid_t& begin_state)
{
    stop();
    return start(begin_state);
}

bool engine::start(const grid_t& begin_state)
{
    m_grid.resize(m_row_count);
    std::for_each(m_grid.begin(), m_grid.end(), [this](row_t& r) { r.resize(m_col_count, false); });

    for (size_t r = 0; r < begin_state.size(); ++r) {
        const row_t& row = begin_state[r];
        for (size_t c = 0; c < row.size(); ++c) {
            m_grid[r][c] = begin_state[r][c];
        }
    }

    return true;
}

void engine::stop()
{
    for (row_t& r : m_grid) {
        std::fill(r.begin(), r.end(), false);
    }
}

} // namespace life

