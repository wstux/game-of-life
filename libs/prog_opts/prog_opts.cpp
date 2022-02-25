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

#include "prog_opts/prog_opts.h"

namespace po {

//////////////////////////////////////////////////////////////////////
// class prog_opts::arg

bool prog_opts::arg::set_value(const char* p_arg)
{
    if (! (bool)p_value) {
        p_value = std::make_shared<value_t>();
    }
    return cvt(*p_value, p_arg);
}

//////////////////////////////////////////////////////////////////////
// class prog_opts

bool prog_opts::has_key(const std::string& po) const
{
    return (m_tokens.find(po) != m_tokens.cend());
}

bool prog_opts::has_value(const std::string& po) const
{
    tokens_list_t::const_iterator it = m_tokens.find(po);
    if (it == m_tokens.cend()) {
        return false;
    }
    return (bool)it->second->p_value;
}

bool prog_opts::insert_impl(const arg::ptr& a)
{
    if (has_error()) {
        return false;
    }

    if (a->po.size() < 3) {
        m_error_msg = "Invalid program option '" + a->po + "'";
        return false;
    }
    if (a->po[2] != ',') {
        m_error_msg = "Invalid program option separator '" + a->po + "'";
        return false;
    }
    if (! m_tokens.emplace(a->long_po(), a).second) {
        m_error_msg = "Exists program option '" + a->po + "'";
        return false;
    }
    if (! m_tokens.emplace(a->short_po(), a).second) {
        m_error_msg = "Exists program option '" + a->po + "'";
        return false;
    }
    return true;
}

bool prog_opts::parse(const int argc, char const* const* argv)
{
    if (has_error()) {
        return false;
    }

    if (argc < 1) {
        m_error_msg = "Invalid argc value: '" + std::to_string(argc) + "'";
        return false;
    }
    m_app = argv[0];

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg_view(argv[i]);
        tokens_list_t::iterator it = m_tokens.find(arg_view);
        if (it == m_tokens.cend()) {
            m_error_msg = "Unsupported key: '" + std::string(arg_view) + "'";
            return false;
        }
        arg& a = *(it->second);
        if ((i + 1) < argc) {
            if (m_tokens.find(std::string_view(argv[i + 1])) == m_tokens.cend()) {
                ++i;
                if (! a.set_value(argv[i])) {
                    m_error_msg = "Invalid value '" + std::string(argv[i]) + "' for arg: '" + std::string(arg_view) + "'";
                    return false;
                }
            } else if (! a.set_value("true")) {
                m_error_msg = "Invalid value for arg: '" + std::string(arg_view) + "'";
                return false;
            }            
        } else {
            if (! a.set_value("true")) {
                m_error_msg = "Invalid value for arg: '" + std::string(arg_view) + "'";
                return false;
            }
        }
    }
    return true;
}

} // namespace po

