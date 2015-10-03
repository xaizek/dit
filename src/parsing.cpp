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

#include <regex>
#include <string>
#include <utility>
#include <vector>

#include <boost/fusion/adapted/struct.hpp>
#include <boost/spirit/include/qi_attr.hpp>
#include <boost/spirit/include/qi_char_.hpp>
#include <boost/spirit/include/qi_char_class.hpp>
#include <boost/spirit/include/qi_core.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/tokenizer.hpp>

#include "decoration.hpp"

namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

using qi::alnum;
using qi::alpha;
using qi::char_;
using qi::lexeme;

// Make Boost.Fusion aware of structures so they can be inferred automatically
// from parsing rules.
BOOST_FUSION_ADAPT_STRUCT(
    Cond,
    (std::string, key)
    (Op, op)
    (std::string, value)
)
BOOST_FUSION_ADAPT_STRUCT(
    ColorRule,
    (std::vector<ColorRule::decoration>, decors)
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
static class dec_ : public qi::symbols<char, ColorRule::decoration>
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
        expr  %= key >> op >> value;
        key   %= ::key;
        value %= lexeme[ *char_ ];
    }

private:
    /**
     * @brief Whole expression: expr ::= key op value
     *
     * Where: op ::= "==" | "!=" | "/" | "=/" | "#" | "!/"
     */
    qi::rule<I, Cond(), ascii::space_type> expr;
    /**
     * @brief Key name: key ::= [a-zA-Z_] [-a-zA-Z_]*
     */
    qi::rule<I, std::string(), ascii::space_type> key;
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
        using qi::attr;

        rules   %= -(rule >> *(';' >> rule));
        rule    %= +dec >> +match;
        match   %= special | cond;
        special %= string("!heading") >> attr(Op::eq) >> attr(std::string());
        // Not reusing CondParser here because it parses till the end of the
        // string.
        cond    %= ::key >> op >> lexeme[ *(char_ - ' ' - '\t' - ';') ];
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

static std::vector<std::string> applyAlias(
    const std::vector<std::string> &alias, std::vector<std::string> &args,
    bool completion);

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
            if (state == State::FIRST) {
                // Remove trailing '=' from item so that its value is not
                // erased.
                parsed.back().pop_back();
            }

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

    if (state == State::FIRST) {
        // Remove trailing '=' from item so that its value is not
        // erased.
        parsed.back().pop_back();
    }

    return std::move(parsed);
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

std::string
parseInvocation(std::vector<std::string> &args,
                std::function<std::string(const std::string &)> aliasResolver,
                bool completion)
{
    std::string cmdName;
    if (!args.empty()) {
        cmdName = args[0];
        args.erase(args.begin());
    }

    const std::string aliasRHS = aliasResolver(cmdName);
    if (!aliasRHS.empty()) {
        const std::vector<std::string> &alias = breakIntoArgs(aliasRHS);
        args = applyAlias(alias, args, completion);

        if (args.empty()) {
            cmdName.clear();
        } else {
            cmdName = args[0];
            args.erase(args.begin());
        }
    }

    return std::move(cmdName);
}

/**
 * @brief Performs alias argument substitution.
 *
 * @param alias Right-hand side of the alias.
 * @param[in,out] args Arguments passed to the alias.
 * @param completion Whether we're performing completion.
 *
 * @returns Name of the command to execute.
 */
static std::vector<std::string>
applyAlias(const std::vector<std::string> &alias,
           std::vector<std::string> &args, bool completion)
{
    static std::regex argRegex(R"(\$\{(\d+)\})");

    std::vector<bool> argUsed(args.size());

    std::vector<std::string> substituted;

    std::smatch match;
    for (const std::string &arg : alias) {
        if (std::regex_match(arg, match, argRegex)) {
            const unsigned int argN = std::stoi(match[1].str());
            if (argN == 0) {
                substituted.push_back(arg);
            } else if (argN <= args.size()) {
                substituted.push_back(args[argN - 1U]);
                argUsed[argN - 1U] = true;

                // Stop after inserting last argument during completion.
                if (completion && argN == args.size()) {
                    return std::move(substituted);
                }
            } else {
                substituted.emplace_back();
            }
        } else {
            substituted.push_back(arg);
        }
    }

    // Append all unused elements.
    for (unsigned int i = 0; i < args.size(); ++i) {
        if (!argUsed[i]) {
            substituted.push_back(args[i]);
        }
    }

    return std::move(substituted);
}

std::vector<std::string>
breakIntoArgs(const std::string &line)
{
    boost::escaped_list_separator<char> sep("\\", " ", "\"'");
    boost::tokenizer<boost::escaped_list_separator<char>> tok(line, sep);
    return std::vector<std::string>(tok.begin(), tok.end());
}
