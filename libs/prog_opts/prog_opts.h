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

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

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
            , is_switch(std::is_same<T, bool>::value)
        {
            if (is_switch && ! p_value) {
                p_value = std::make_shared<value_t>(false);
            }
        }

        bool has_value() const { return (p_value.get() == nullptr); }
        std::string_view long_po() const { return std::string_view(po.data() + 3, po.size() - 3); }
        std::string printable_po() const;
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
        bool is_switch;
    };

    using args_list_t = std::vector<arg::ptr>;
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
    std::string m_error_msg;
    args_list_t m_supported_args;
    tokens_list_t m_tokens;
};

} // namespace po

