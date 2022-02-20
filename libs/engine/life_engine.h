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

