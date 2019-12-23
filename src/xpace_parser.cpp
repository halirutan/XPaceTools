#include "xpace_parser.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(xpace::parser::initial_pose_t,
                          (double, x),
                          (double, y),
                          (double, z),
                          (double, qi),
                          (double, qj),
                          (double, qk),
                          (double, qr))

BOOST_FUSION_ADAPT_STRUCT(xpace::parser::motion_t,
                          (int, frame),
                          (int, time),
                          (double, x),
                          (double, y),
                          (double, z),
                          (double, qi),
                          (double, qj),
                          (double, qk),
                          (double, qr))


namespace xpace
{
namespace parser
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

template<typename Iterator>
struct initial_pose_parser: qi::grammar<Iterator, initial_pose_t(), ascii::space_type>
{
    initial_pose_parser()
        : initial_pose_parser::base_type(start)
    {
        using ascii::char_;
        using qi::double_;
        using qi::repeat;
        using qi::eol;

        start %=
            '@'
                >> double_
                >> double_
                >> double_
                >> '('
                >> double_
                >> double_
                >> double_
                >> ')'
                >> double_;
    }

    qi::rule<Iterator, initial_pose_t(), ascii::space_type> start;
};

template<typename Iterator>
struct motion_parser: qi::grammar<Iterator, motion_t(), ascii::space_type>
{
    motion_parser()
        : motion_parser::base_type(start)
    {
        using ascii::char_;
        using qi::double_;
        using qi::int_;
        using qi::repeat;
        using qi::eol;
        using qi::omit;

        start %=
                int_
                >> int_
                >> double_
                >> double_
                >> double_
                >> '('
                >> double_
                >> double_
                >> double_
                >> ')'
                >> double_
                >> omit[
                    char_
                        >> repeat(2)[int_]
                        >> char_
                ];
    }

    qi::rule<Iterator, motion_t(), ascii::space_type> start;
};

bool parseInitialPose(const std::string &input, initial_pose_t &out) {
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef initial_pose_parser<iterator_type> parser_type;

    parser_type myParser;
    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();
    bool result = phrase_parse(iter, end, myParser, space, out);
    return result && (iter == end);
}

bool parseMotion(const std::string &input, motion_t &out) {
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef motion_parser<iterator_type> parser_type;

    parser_type myParser;
    std::string::const_iterator iter = input.begin();
    std::string::const_iterator end = input.end();
    bool result = phrase_parse(iter, end, myParser, space, out);
    return result && (iter == end);
}
}
}