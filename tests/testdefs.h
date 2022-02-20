#ifndef TESTING_TESTDEFS_H
#define TESTING_TESTDEFS_H

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

namespace tests {

class timer final
{
    using time_point = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
    timer(bool run = false)
    {
        if (run) {
            start();
        }
    }

    void pause()
    {
        m_time_ms = value_ms();
        is_start = false;
    }
    
    void start()
    {
        is_start = true;
        m_start = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        is_start = false;
        m_time_ms = 0.0;
    }

    double value_ms()
    {
        if (is_start) {
            time_point cur = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> ms_double = cur - m_start;
            m_time_ms += ms_double.count();
        }
        return m_time_ms;
    }
    
private:
    bool is_start = false;
    time_point m_start;
    double m_time_ms = 0.0;
};

namespace details {

template<size_t N, typename TTuple, template<typename> typename TInsertType, template<typename> class TPtr, typename TType>
struct add
{
    static void call(std::vector<TPtr<TType>>& cont)
    {
        add<N - 1, TTuple, TInsertType, TPtr, TType>::call(cont);
        cont.emplace_back(std::make_shared<TInsertType<std::tuple_element_t<N, TTuple>>>());
    }
};

template<typename TTuple, template<typename> typename TInsertType, template<typename> class TPtr, typename TType>
struct add<0, TTuple, TInsertType, TPtr, TType>
{
    static void call(std::vector<TPtr<TType>>& cont)
    {
        cont.emplace_back(std::make_shared<TInsertType<std::tuple_element_t<0, TTuple>>>());
    }
};

template<typename TTuple, template<typename> typename TInsertType, template<typename> class TPtr, typename TType>
void emplace(std::vector<TPtr<TType>>& cont)
{
    add<std::tuple_size<TTuple>::value - 1, TTuple, TInsertType, TPtr, TType>::call(cont);
}

class tester final
{
public:
    class itest_suite
    {
    public:
        using ptr = std::shared_ptr<itest_suite>;

        virtual void test_body() = 0;
    };

    using case_name_t = std::string;
    using test_name_t = std::string;
    using type_name_t = std::string;
    using test_descr_t = std::pair<test_name_t, itest_suite::ptr>;
    using test_list_t = std::vector<test_descr_t>;

    class test_case final
    {
    public:
        using ptr = std::shared_ptr<test_case>;

        explicit test_case(const case_name_t& case_name)
            : m_case_name(case_name)
        {}

        bool insert_case(const test_name_t& test_name, const itest_suite::ptr& p_suite)
        {
            m_tests.emplace_back(test_name, p_suite);
            return true;
        }

        int run_all_tests() const
        {
            const std::string prefix = std::to_string(m_tests.size()) + " tests from " + m_case_name;
        
            std::cout << "[----------] " << prefix << std::endl;
            timer suite_sw(true);
            const int failed_count = run_tests();
            const double suite_ms = suite_sw.value_ms();
            std::cout << "[----------] " << prefix << " (" << suite_ms << " ms)" << std::endl << std::endl;

            return failed_count;
        }

        size_t tests_count() const { return m_tests.size(); }

    private:
        int run_tests() const
        {
            int failed_count = 0;
            for (const test_descr_t& descr : m_tests) {
                const test_name_t& test_name = descr.first;
                const itest_suite::ptr& p_suite = descr.second;

                tester::begin_test();
                std::cout << "[RUN       ] " << m_case_name << "." << test_name << std::endl;

                timer test_sw(true);
                p_suite->test_body();
                const double test_ms = test_sw.value_ms();

                failed_count += (tester::end_test() != 0) ? 1 : 0;
                const std::string res_str = (tester::end_test() == 0) ? "[       OK ] " : "[   FAILED ] ";
                std::cout << res_str << m_case_name << "." << test_name << " (" << test_ms << " ms)" << std::endl;
            }

            return failed_count;
        }

    private:
        const case_name_t m_case_name;
        test_list_t m_tests;
    };

    static void begin_suite() { m_ut_suite_result = 0; }
    
    static void begin_test() { m_ut_test_result = 0; }

    static int end_suite() { return m_ut_suite_result; }

    static int end_test() { return m_ut_test_result; }

    static void fail()
    {
        m_ut_result = 1;
        m_ut_suite_result = 1;
        m_ut_test_result = 1;
    }

    static bool insert(const std::string& case_name, const std::string& test_name, const itest_suite::ptr& p_suite)
    {
        std::map<case_name_t, size_t>::iterator it = m_case_names.find(case_name);
        if (it == m_case_names.cend()) {
            it = m_case_names.emplace(case_name, m_tests.size()).first;
            m_tests.emplace_back(std::make_shared<test_case>(case_name));
        }

        const size_t idx = it->second;
        return m_tests[idx]->insert_case(test_name, p_suite);
    }

    static int run_all_tests()
    {
        const size_t tests_cnt = tests_count();
        const size_t suits_count = 1;

        std::cout << "[==========] Running " << tests_cnt << " tests from " << suits_count << " test suits." << std::endl;
        timer total_sw(true);
        for (const test_case::ptr& p_test : m_tests) {
            p_test->run_all_tests();
        }

        const double total_ms = total_sw.value_ms();
        std::cout << "[==========] " << tests_cnt << " tests from " << suits_count << " test suits ran (" << total_ms
                  << " ms)." << std::endl;
        std::cout << "[  PASSED  ] " << tests_cnt << " tests." << std::endl;

        return tester::ut_result();
    }

    static int ut_result() { return m_ut_result; }

private:
    tester() {}

    static size_t tests_count()
    {
        return std::accumulate(m_tests.cbegin(), m_tests.cend(), 0, [](size_t a, const test_case::ptr& b) -> size_t {
                                                                        return a + b->tests_count();
                                                                    });
    }

private:
    static int m_ut_result;
    static int m_ut_suite_result;
    static int m_ut_test_result;

    static std::map<case_name_t, size_t> m_case_names;
    static std::vector<test_case::ptr> m_tests;
};

int tester::m_ut_result = 0;
int tester::m_ut_suite_result = 0;
int tester::m_ut_test_result = 0;
std::map<tester::case_name_t, size_t> tester::m_case_names = std::map<tester::case_name_t, size_t>();
std::vector<tester::test_case::ptr> tester::m_tests = std::vector<tester::test_case::ptr>();

std::ostream& fail()
{
    tester::fail();
    return std::cerr;
}

template<typename TTuple>
class typed_tester
{
    using case_name_t = std::string;
    using case_index_t = size_t;
    using type_name_t = std::string;
    using test_name_t = std::string;
    using test_descr_t = std::pair<test_name_t, tester::itest_suite::ptr>;
    using test_list_t = std::vector<test_descr_t>;
    using case_list_t = std::map<case_index_t, test_list_t>;

public:
    explicit typed_tester(const std::string& t1,
                          const std::string& t2 = std::string(), const std::string& t3 = std::string(),
                          const std::string& t4 = std::string(), const std::string& t5 = std::string(),
                          const std::string& t6 = std::string(), const std::string& t7 = std::string())
        : m_test_types(make_types_list(t1, t2, t3, t4, t5, t6, t7))
    {}

    template<template<typename> typename TTestFn>
    bool insert_test_case(const std::string& case_name, const std::string& test_name)
    {
        std::vector<tester::itest_suite::ptr> test_suites;
        emplace<TTuple, TTestFn>(test_suites);

        for (size_t i = 0; i < m_test_types.size(); ++i) {
            const std::string test_case_name = case_name + "<" + m_test_types[i] + ">";
            tester::insert(test_case_name, test_name, test_suites[i]);
        }

        return true;
    }

private:
    static std::vector<type_name_t> make_types_list(const std::string& t1, const std::string& t2, const std::string& t3,
                                                    const std::string& t4, const std::string& t5, const std::string& t6,
                                                    const std::string& t7)
    {
        const std::vector<type_name_t> tmp_list = {t1, t2, t3, t4, t5, t6, t7};

        std::vector<type_name_t> types_list;
        for (const std::string& t : tmp_list) {
            if (t.empty()) {
                continue;
            }
            types_list.emplace_back(t);
        }
        return types_list;
    }

private:
    const std::vector<type_name_t> m_test_types;
};

} // namespace details
} // namespace tests

#define __CVT_TO_STRING(x) \
    []() -> std::string { return #x; }()

#define __FAILURE_MESSAGE(cond) \
    ::tests::details::fail() << __FILE__ << ":" << __LINE__ << ":" << std::endl \
                             << "    " << __PRETTY_FUNCTION__ << ":" << std::endl \
                             << "Failure condition '" << #cond << "'" << std::endl

#define __GEN_TEST_CASE_NAME(suite_name) \
    __g_private_##suite_name##_test

#define __GEN_TEST_CLASS_NAME(suite_name, test_name) \
    suite_name##_##test_name##_test

#define __TO_UTF8_STRING(x) x
#define __TO_WSTRING(x) L ## x
#define __TO_UTF16_STRING(x) u ## x
#define __TO_UTF32_STRING(x) U ## x

#define INIT_TYPE_TESTS(case_name, t1, t2, t3, t4, t5, t6, t7) \
    namespace { \
        using __private_test_types_list = std::tuple<t1, t2, t3, t4, t5, t6, t7>; \
        ::tests::details::typed_tester<__private_test_types_list> \
            __g_private_tester(#t1, #t2, #t3, #t4, #t5, #t6, #t7); \
    }

#define TEST(case_name, test_name) \
    class __GEN_TEST_CLASS_NAME(case_name, test_name) \
        : public ::tests::details::tester::itest_suite \
    { \
    public: \
        virtual void test_body() override final; \
    }; \
    [[maybe_unused]] static bool __##case_name##_##test_name##_res_ = \
            ::tests::details::tester::insert( \
                __CVT_TO_STRING(case_name), __CVT_TO_STRING(test_name), \
                std::make_shared<__GEN_TEST_CLASS_NAME(case_name, test_name)>()); \
    void __GEN_TEST_CLASS_NAME(case_name, test_name)::test_body()

#define TYPED_TEST(case_name, test_name) \
    template<typename T> \
    class __GEN_TEST_CLASS_NAME(case_name, test_name) \
        : public ::tests::details::tester::itest_suite \
    { \
    public: \
        using TType = T; \
        virtual void test_body() override final; \
    }; \
    [[maybe_unused]] static bool __##case_name##_##test_name##_res_ = \
            __g_private_tester.insert_test_case<__GEN_TEST_CLASS_NAME(case_name, test_name)>( \
                __CVT_TO_STRING(case_name), __CVT_TO_STRING(test_name)); \
    template<typename T> \
    void __GEN_TEST_CLASS_NAME(case_name, test_name)<T>::test_body()

#define EXPECTED(cond) \
    if ((cond)) ;\
    else __FAILURE_MESSAGE(cond)

#define EXPECTED_EQ(et, td) \
    if ((et.size() == td.size()) && (std::equal(et.begin(), et.end(), td.begin()))) ;\
    else __FAILURE_MESSAGE(cond)

#define RUN_TESTS() ::tests::details::tester::run_all_tests()

#define U(x) \
    []() -> std::basic_string<TChar> { \
        if constexpr (std::is_same<TChar, wchar_t>::value) { \
            return reinterpret_cast<const TChar*>(__TO_WSTRING(x)); \
        } else if constexpr (sizeof(TChar) == 1) { \
            return reinterpret_cast<const TChar*>(__TO_UTF8_STRING(x)); \
        } else if constexpr (sizeof(TChar) == 2) { \
            return reinterpret_cast<const TChar*>(__TO_UTF16_STRING(x)); \
        } else /* if constexpr (sizeof(TChar) == 4) */ { \
            return reinterpret_cast<const TChar*>(__TO_UTF32_STRING(x)); \
        } \
    }()

#endif // TESTING_TESTDEFS_H

