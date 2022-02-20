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

