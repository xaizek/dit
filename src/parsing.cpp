// Copyright (C) 2015 xaizek <xaizek@posteo.net>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of version 3 of the GNU Affero General Public
// License as published by the Free Software Foundation.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include "parsing.hpp"

#include <string>
#include <utility>
#include <vector>

#include <boost/fusion/adapted/struct.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/repository/include/qi_iter_pos.hpp>

#include "decoration.hpp"

namespace ascii = boost::spirit::ascii;
namespace phx = boost::phoenix;
namespace qi = boost::spirit::qi;

using qi::alnum;
using qi::alpha;
using qi::char_;
using qi::lexeme;

// Make Boost.Fusion aware of structures so they can be inferred automatically
// from parsing rules.
BOOST_FUSION_ADAPT_STRUCT(
    Cond,
    (std::string, str)
    (std::string, key)
    (Op, op)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    ColorRule,
    (std::vector<const decor::Decoration *>, decors)
    (std::vector<Cond>, conds)
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
 * @brief Symbol table of decorations.
 */
static class dec_ : public qi::symbols<char, const decor::Decoration *>
{
public:
    dec_()
    {
        add
            ("bold"       , &decor::bold)
            ("inv"        , &decor::inv)
            ("def"        , &decor::def)

            ("fg-black"   , &decor::black_fg)
            ("fg-red"     , &decor::red_fg)
            ("fg-green"   , &decor::green_fg)
            ("fg-yellow"  , &decor::yellow_fg)
            ("fg-blue"    , &decor::blue_fg)
            ("fg-magenta" , &decor::magenta_fg)
            ("fg-cyan"    , &decor::cyan_fg)
            ("fg-white"   , &decor::white_fg)

            ("bg-black"   , &decor::black_bg)
            ("bg-red"     , &decor::red_bg)
            ("bg-green"   , &decor::green_bg)
            ("bg-yellow"  , &decor::yellow_bg)
            ("bg-blue"    , &decor::blue_bg)
            ("bg-magenta" , &decor::magenta_bg)
            ("bg-cyan"    , &decor::cyan_bg)
            ("bg-white"   , &decor::white_bg)
        ;
    }
} dec;

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
     * @brief Constructs and configures the parser.
     */
    CondParser() : CondParser::base_type(expr)
    {
        using boost::spirit::repository::qi::iter_pos;
        using phx::at_c;
        using phx::construct;
        using qi::_1; using qi::_2; using qi::_3; using qi::_4; using qi::_5;
        using qi::_val;

        expr  = ( iter_pos >> ::key >> op >> value >> iter_pos )
                [ at_c<1>(_val) = _2, at_c<2>(_val) = _3, at_c<3>(_val) = _4,
                  at_c<0>(_val) = construct<std::string>(_1, _5) ];
        value %= lexeme[ *char_ ];
    }

private:
    /**
     * @brief Whole expression: expr ::= ::key op value
     *
     * Where: op ::= "==" | "!=" | "/" | "=/" | "#" | "!/"
     */
    qi::rule<I, Cond(), ascii::space_type> expr;
    /**
     * @brief Value to compare with: value ::= .*
     */
    qi::rule<I, std::string(), ascii::space_type> value;
};

/**
 * @brief Parser of colorization specification.
 *
 * @tparam I Type of iterator used to consume input.
 */
template <typename I>
class ColorRulesParser :
    public qi::grammar<I, std::vector<ColorRule>(), ascii::space_type>
{
public:
    /**
     * @brief Constructs and configures the parser.
     */
    ColorRulesParser() : ColorRulesParser::base_type(rules)
    {
        using ascii::string;
        using boost::spirit::repository::qi::iter_pos;
        using phx::at_c;
        using phx::begin;
        using phx::construct;
        using phx::end;
        using qi::_1; using qi::_2; using qi::_3; using qi::_4; using qi::_5;
        using qi::_val;
        using qi::attr;

        rules   %= -(rule >> *(';' >> rule));
        rule    %= +dec >> +match;
        match   %= special | cond;
        special %= attr(std::string())
                >> string("!heading") >> attr(Op::eq) >> attr(std::string());
        // Not reusing CondParser here because it parses till the end of the
        // string.
        cond = ( iter_pos
              >> ::key >> op >> lexeme[ *(char_ - ' ' - '\t' - ';') ]
              >> iter_pos
               )
               [ at_c<1>(_val) = _2, at_c<2>(_val) = _3,
                 at_c<3>(_val) = construct<std::string>(begin(_4), end(_4)),
                 at_c<0>(_val) = construct<std::string>(_1, _5) ];
    }

private:
    /**
     * @brief Whole expression: rules ::= [ rule (';' rule)+ ]
     */
    qi::rule<I, std::vector<ColorRule>(), ascii::space_type> rules;
    /**
     * @brief Single rule: rule ::= dec+ match+
     */
    qi::rule<I, ColorRule(), ascii::space_type> rule;
    /**
     * @brief Single item matcher: match ::= special | cond
     */
    qi::rule<I, Cond(), ascii::space_type> match;
    /**
     * @brief Pseudo match item: special ::= "!heading"
     */
    qi::rule<I, Cond(), ascii::space_type> special;
    /**
     * @brief Item condition: cond ::= key op value
     *
     * Where:
     *  - op ::= "==" | "!=" | "/" | "=/" | "#" | "!/"
     *  - value ::= [^ \t;]*
     */
    qi::rule<I, Cond(), ascii::space_type> cond;
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

std::vector<std::string>
parsePairedArgs(const std::vector<std::string> &args)
{
    enum class State { REGULAR, FIRST, APPEND, };

    std::vector<std::string> parsed;

    State state = State::REGULAR;
    for (const std::string &arg : args) {
        // Assignment with valid key name resets parsing to initial state.
        const std::string::size_type eqPos = arg.find('=');
        if (eqPos != std::string::npos) {
            auto iter = arg.cbegin();
            if (parseKeyName(iter, iter + eqPos)) {
                parsed.emplace_back(arg);
                state = State::REGULAR;
                continue;
            }
        }

        if (arg.size() > 1U && arg.back() == ':') {
            parsed.emplace_back(arg.begin(), --arg.end());
            parsed.back() += '=';
            state = State::FIRST;
        } else if (state == State::FIRST) {
            parsed.back().append(arg);
            state = State::APPEND;
        } else if (state == State::APPEND) {
            parsed.back().append(' ' + arg);
        } else {
            parsed.emplace_back(arg);
        }
    }

    return parsed;
}

bool
parseColorRules(const std::string &spec, std::vector<ColorRule> &colorRules)
{
    std::string::const_iterator iter = spec.cbegin();
    ColorRulesParser<std::string::const_iterator> grammar;
    return qi::phrase_parse(iter, spec.cend(), grammar, ascii::space,
                            colorRules)
        && iter == spec.cend();
}
