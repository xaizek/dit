// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of scribe.
//
// scribe is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// scribe is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with scribe.  If not, see <http://www.gnu.org/licenses/>.

#include "parsing.hpp"

#include <string>

#include <boost/fusion/functional.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>

namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

using qi::alnum;
using qi::alpha;
using qi::char_;
using qi::lexeme;

// Make Boost.Fusion aware of the structure so it can be composed automatically.
BOOST_FUSION_ADAPT_STRUCT(
    Cond,
    (std::string, key)
    (Op, op)
    (std::string, value)
)

/**
 * @brief Symbol table of comparison operators.
 */
static class op_ : public qi::symbols<char, Op>
{
public:
    op_()
    {
        add
            ("=="  , Op::eq)
            ("!="  , Op::ne)
            // TODO:
            // ("/"   , Op::contains)
            // ("#"   , Op::notcontain)
            // ("//"  , Op::matches)
            // ("##"  , Op::notmatch)
            // OR ("ic" -- ignore case)
            ("/"   , Op::iccontains)
            ("=/"  , Op::iccontains)
            ("#"   , Op::icnotcontain)
            ("!/"  , Op::icnotcontain)
            // ("//"  , Op::contains)
            // ("##"  , Op::notcontains)
            // ("=~"  , Op::icmatches)
            // ("!~"  , Op::icnotmatch)
            // ("=~~" , Op::matches)
            // ("!~~" , Op::notmatch)
        ;
    }
} op;

/**
 * @brief key name: key ::= [a-zA-Z_] [-a-zA-Z_]*
 */
static qi::rule<std::string::const_iterator, std::string(), ascii::space_type>
    key = lexeme[ (alpha | char_('_')) >>
                 *(alnum | char_('_') | char_('-')) ];

namespace {

/**
 * @brief Parser of a single conditional expression.
 *
 * @tparam I Type of iterator used to consume input.
 */
template <typename I>
class CondParser : public qi::grammar<I, Cond(), ascii::space_type>
{
public:
    /**
     * @brief Constructs the parser.
     */
    CondParser() : CondParser::base_type(expr)
    {
        expr  %= key >> op >> value;
        key   %= ::key;
        value %= lexeme[ *char_ ];
    }

private:
    /**
     * @brief Whole expression: expr ::= key op value
     *
     * Where: op ::= "==" | "!="
     */
    qi::rule<I, Cond(), ascii::space_type> expr;
    /**
     * @brief key name: key ::= [a-zA-Z_] [-a-zA-Z_]*
     */
    qi::rule<I, std::string(), ascii::space_type> key;
    /**
     * @brief Value to compare with: value ::= .*
     */
    qi::rule<I, std::string(), ascii::space_type> value;
};

}

bool
parseKeyName(std::string::const_iterator &iter, std::string::const_iterator end)
{
    return qi::phrase_parse(iter, end, key, ascii::space)
        && iter == end;
}

bool
parseCond(std::string::const_iterator &iter, std::string::const_iterator end,
          Cond &cond)
{
    CondParser<std::string::const_iterator> grammar;
    return qi::phrase_parse(iter, end, grammar, ascii::space, cond)
        && iter == end;
}
