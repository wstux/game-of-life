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

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace po {

class prog_opts final
{
    using value_t = std::variant<bool, int, double, std::string>;
    using value_ptr_t = std::shared_ptr<value_t>;
    using cvt_t = std::function<bool(value_t&, const char*)>;

    template<typename> static inline constexpr bool false_v = false;

    struct arg final
    {
        using ptr = std::shared_ptr<arg>;

        arg() {}

        template<typename T>
        arg(const std::string& p, const std::string& d, const T* v)
            : cvt(create_cvt<T>())
            , po(p)
            , descr(d)
            , p_value((v == nullptr) ? value_ptr_t() : std::make_shared<value_t>(*v))
        {}

        bool has_value() const { return (p_value.get() == nullptr); }
        std::string_view long_po() const { return std::string_view(po.data() + 3, po.size() - 3); }
        bool set_value(const char* p_arg);
        std::string_view short_po() const { return std::string_view(po.data(), 2); }

        template<typename T>
        static cvt_t create_cvt()
        {
            cvt_t c;
            if constexpr (std::is_same<T, bool>::value) {
                c = [] (value_t& v, const char* p_arg) -> bool {
                    if ((std::string_view(p_arg) != "true") && (std::string_view(p_arg) != "true")) {
                        return false;
                    }
                    v = (std::string_view(p_arg) == "true");
                    return true;
                };
            } else if constexpr (std::is_same<T, int>::value) {
                c = [] (value_t& v, const char* p_arg) ->bool {
                    const std::string s = p_arg;
                    size_t processed = 0;
                    const int i = std::stoi(s, &processed);
                    if (s.size() != processed) {
                        return false;
                    }
                    v = i;
                    return true;
                };
            } else if constexpr (std::is_same<T, double>::value) {
                c = [] (value_t& v, const char* p_arg) ->bool {
                    const std::string s = p_arg;
                    size_t processed = 0;
                    const double d = std::stod(s, &processed);
                    if (s.size() != processed) {
                        return false;
                    }
                    v = d;
                    return true;
                };
            } else if constexpr (std::is_same<T, std::string>::value) {
                c = [] (value_t& v, const char* p_arg) -> bool {
                    v = std::string(p_arg);
                    return true;
                };
            } else {
                static_assert(false_v<T>, "unsupported visitor");
            }
            return c;
        }

        const cvt_t cvt;
        std::string po;
        std::string descr;
        value_ptr_t p_value;
    };

    using tokens_list_t = std::unordered_map<std::string_view, arg::ptr>;

public:
    const std::string& error_msg() const { return m_error_msg; }

    bool has_error() const { return (! m_error_msg.empty()); }

    bool has_value(const std::string& po) const;

    template<typename TType>
    bool insert(const std::string& po, const std::string& descr)
    {
        arg::ptr a = std::make_shared<arg>(po, descr, (TType*)nullptr);
        return insert_impl(a);
    }

    template<typename TType>
    bool insert(const std::string& po, const TType& def_value, const std::string& descr)
    {
        arg::ptr a = std::make_shared<arg>(po, descr, &def_value);
        return insert_impl(a);
    }

    bool parse(const int argc, char const* const* argv);

    template<typename TType>
    const TType& value(const std::string& po) const
    {
        const arg::ptr& a = m_tokens.at(po);
        return std::get<TType>(*(a->p_value));
    }

    std::string usage() const;

private:
    bool has_key(const std::string& po) const;

    bool insert_impl(const arg::ptr& a);

private:
    std::string m_app;
    std::string m_error_msg;
    tokens_list_t m_tokens;
};

} // namespace po

