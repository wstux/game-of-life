#include <string>
#include <vector>

#include "prog_opts/prog_opts.h"

#include "testdefs.h"

namespace {

std::vector<char*> cvt_to_argv(const std::vector<std::string>& po_list)
{
    std::vector<char*> argv_list;
    argv_list.reserve(po_list.size());
    for (const std::string& po : po_list) {
        argv_list.push_back((char*)po.data());
    }
    return argv_list;
}

} // <anonymous> namespace

TEST(prog_opts, insert)
{
    po::prog_opts po;

    EXPECTED(po.insert("-h,--help", false, "pring help message")) << po.error_msg() << std::endl;
    EXPECTED(! po.insert("-h|--help", false, "pring help message"));
    EXPECTED(! po.insert("-f|--file", false, "pring help message"));
}

TEST(prog_opts, parse_po)
{
    const std::vector<std::string> supported_po_list = {"./ut_prog_opts", "-h", "--help", "-f", "--file"};
    const std::vector<std::string> unsupported_po_list = {"./ut_prog_opts", "-i", "--stream"};

    po::prog_opts po;

    EXPECTED(po.insert<bool>("-h,--help", "pring help message")) << po.error_msg() << std::endl;
    EXPECTED(po.insert<bool>("-f,--file", "pring help message")) << po.error_msg() << std::endl;

    std::vector<char*> sup_argv_list = cvt_to_argv(supported_po_list);
    EXPECTED(po.parse(5, sup_argv_list.data())) << po.error_msg() << std::endl;

    std::vector<char*> unsup_argv_list = cvt_to_argv(unsupported_po_list);
    EXPECTED(! po.parse(3, unsup_argv_list.data()));
}

TEST(prog_opts, check_keys)
{
    const std::vector<std::string> po_list = {"./ut_prog_opts", "-h", "--file", "15", "-s", "bugaga", "-t", "0.3"};

    po::prog_opts po;
    EXPECTED(po.insert<bool>("-h,--help", "1")) << po.error_msg() << std::endl;
    EXPECTED(po.insert<int>("-f,--file", "1")) << po.error_msg() << std::endl;
    EXPECTED(po.insert<double>("-t,--total", "1")) << po.error_msg() << std::endl;
    EXPECTED(po.insert<std::string>("-s,--star", "1")) << po.error_msg() << std::endl;

    std::vector<char*> argv_list = cvt_to_argv(po_list);
    EXPECTED(po.parse(argv_list.size(), argv_list.data())) << po.error_msg() << std::endl;

    EXPECTED(po.has_value("--help"));
    EXPECTED(po.has_value("--file"));
    EXPECTED(po.has_value("--total"));
    EXPECTED(po.has_value("--star"));
    EXPECTED(po.has_value("-h"));
    EXPECTED(po.has_value("-f"));
    EXPECTED(po.has_value("-t"));
    EXPECTED(po.has_value("-s"));
    EXPECTED(! po.has_value("--stream"));

    if (po.has_value("-h")) {
        EXPECTED(po.value<bool>("--help"));
    }
    if (po.has_value("-f")) {
        EXPECTED(po.value<int>("--file") == 15);
    }
    if (po.has_value("-t")) {
        EXPECTED(po.value<double>("--total") == 0.3);
    }
    if (po.has_value("-s")) {
        EXPECTED(po.value<std::string>("--star") == "bugaga");
    }
}

int main()
{
    return RUN_TESTS();
}

