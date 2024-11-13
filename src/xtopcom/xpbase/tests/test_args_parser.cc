#include <gtest/gtest.h>

#define private public
#define protected public
#include "xpbase/base/args_parser.h"

namespace tcash {
namespace test {

class TestArgsParser : public testing::Test {
public:
    static void SetUpTestCase() {
    }

    static void TearDownTestCase() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(TestArgsParser, WordDup) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMaybeValue);

    std::string tmp_params1("-h -g 0 -p peer ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, NoWord) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMaybeValue);

    std::string tmp_params1("-f -g 0 -p peer ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, NoValue) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMaybeValue);

    std::string tmp_params1("-f -g 0 -p");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, MustValue) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMustValue);

    std::string tmp_params1("-f -g 0 -p 127.0.0.1:100 -t ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, MustNoValue) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMustValue);

    std::string tmp_params1("-h test_help ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, SpecialCharStar) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMustValue);

    std::string tmp_params1("-h test_help\" ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, SpecialCharQuote) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMustValue);

    std::string tmp_params1("-h test_help\" ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
}

TEST_F(TestArgsParser, AddArgType) {
    tcash::ArgsParser args_parser;
    args_parser.AddArgType('h', "help", tcash::kNoValue);
    args_parser.AddArgType('g', "show_cmd", tcash::kMaybeValue);
    args_parser.AddArgType('p', "peer", tcash::kMaybeValue);
    args_parser.AddArgType('i', "identity_index", tcash::kMaybeValue);
    args_parser.AddArgType('l', "local_port", tcash::kMaybeValue);
    args_parser.AddArgType('a', "local_ip", tcash::kMaybeValue);
    args_parser.AddArgType('o', "country_code", tcash::kMaybeValue);
    args_parser.AddArgType('u', "business", tcash::kMaybeValue);
    args_parser.AddArgType('c', "config_path", tcash::kMaybeValue);
    args_parser.AddArgType('d', "db_path", tcash::kMaybeValue);
    args_parser.AddArgType('L', "log_path", tcash::kMaybeValue);
    args_parser.AddArgType('t', "test", tcash::kMaybeValue);

    std::string tmp_params1("-h -g 0 -p peer ");
    std::string err_pos1;
    ASSERT_EQ(args_parser.Parse(tmp_params1, err_pos1), tcash::kadmlia::kKadFailed);
    args_parser.result_.clear();
    std::string tmp_params2("-h 1 ");
    ASSERT_EQ(args_parser.Parse(tmp_params2, err_pos1), tcash::kadmlia::kKadFailed);
    args_parser.result_.clear();
    std::string tmp_params3(" ");
    ASSERT_EQ(args_parser.Parse(tmp_params3, err_pos1), tcash::kadmlia::kKadFailed);
    args_parser.result_.clear();
    std::string tmp_params4(" -g g ");
    ASSERT_EQ(args_parser.Parse(tmp_params4, err_pos1), tcash::kadmlia::kKadFailed);
    args_parser.result_.clear();

    std::string tmp_params(
        "-h -g 0 -p 127.0.0.1:1000 -i 1 -l 1000 "
        "-a 127.0.0.1 -o CN -u VPN -c ./test/test.conf "
        "-d ./db -L ./log_path -t 10 ");
    std::string err_pos;
    ASSERT_EQ(args_parser.Parse(tmp_params, err_pos), tcash::kadmlia::kKadSuccess);
    ASSERT_TRUE(args_parser.HasParam("h"));
    int g_val;
    ASSERT_EQ(args_parser.GetParam("g", g_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(g_val, 0);
    std::string p_val;
    ASSERT_EQ(args_parser.GetParam("p", p_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(p_val, "127.0.0.1:1000");
    int i_val;
    ASSERT_EQ(args_parser.GetParam("i", i_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(i_val, 1);
    uint16_t l_val;
    ASSERT_EQ(args_parser.GetParam("l", l_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(l_val, 1000);
    std::string a_val;
    ASSERT_EQ(args_parser.GetParam("a", a_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(a_val, "127.0.0.1");
    std::string o_val;
    ASSERT_EQ(args_parser.GetParam("o", o_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(o_val, "CN");
    std::string u_val;
    ASSERT_EQ(args_parser.GetParam("u", u_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(u_val, "VPN");
    std::string c_val;
    ASSERT_EQ(args_parser.GetParam("c", c_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(c_val, "./test/test.conf");
    std::string d_val;
    ASSERT_EQ(args_parser.GetParam("d", d_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(d_val, "./db");
    std::string L_val;
    ASSERT_EQ(args_parser.GetParam("L", L_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(L_val, "./log_path");
    int t_val;
    ASSERT_EQ(args_parser.GetParam("t", t_val), tcash::kadmlia::kKadSuccess);
    ASSERT_EQ(t_val, 10);
}

}  // namespace test
}  // namespace tcash
