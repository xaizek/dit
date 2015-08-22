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

#include "ItemFilter.hpp"

#include <cassert>

#include <stdexcept>
#include <string>
#include <vector>

#include <boost/fusion/functional.hpp>
#include <boost/spirit/include/qi.hpp>

#include "Item.hpp"

namespace ascii = boost::spirit::ascii;
namespace qi = boost::spirit::qi;

using Cond = ItemFilter::Cond;

/**
 * @brief Enumerations of supported operations.
 */
enum class Op
{
    eq, /**< @brief Check for equality. */
    ne, /**< @brief Check for inequality. */
};

/**
 * @brief Single condition expression.
 */
struct ItemFilter::Cond
{
    std::string key;   /**< @brief Name of the key. */
    Op op;             /**< @brief Operation to use for comparison. */
    std::string value; /**< @brief Value to match against. */
};

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
            ("==" , Op::eq)
            ("!=" , Op::ne)
            // TODO:
            // ("/"   , Op::contains)
            // ("#"   , Op::notcontain)
            // ("//"  , Op::matches)
            // ("##"  , Op::notmatch)
            // OR ("ic" -- ignore case)
            // ("/"   , Op::iccontains)
            // ("#"   , Op::icnotcontain)
            // ("//"  , Op::contains)
            // ("##"  , Op::notcontains)
            // ("=~"  , Op::icmatches)
            // ("!~"  , Op::icnotmatch)
            // ("=~~" , Op::matches)
            // ("!~~" , Op::notmatch)
        ;
    }
} op;

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
        using qi::alnum;
        using qi::alpha;
        using qi::char_;
        using qi::lexeme;

        expr  %= key >> op >> value;
        key   %= lexeme[ (alpha | char_('_')) >>
                        *(alnum | char_('_') | char_('-')) ];
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

ItemFilter::ItemFilter(const std::vector<std::string> &exprs)
{
    CondParser<std::string::const_iterator> g;

    for (const std::string &expr : exprs) {
        std::string::const_iterator iter = expr.begin();
        std::string::const_iterator end = expr.end();
        Cond cond;
        bool r = qi::phrase_parse(iter, end, g, ascii::space, cond);

        if (r && iter == end)
        {
            conds.emplace_back(std::move(cond));
        }
        else
        {
            throw std::runtime_error("Wrong expression: " + expr);
        }
    }
}

ItemFilter::~ItemFilter()
{
}

bool
ItemFilter::passes(Item &item) const
{
    for (const Cond &cond : conds) {
        const std::string &val = item.getValue(cond.key);
        switch (cond.op) {
            case Op::eq:
                if (val != cond.value) {
                    return false;
                }
                break;
            case Op::ne:
                if (val == cond.value) {
                    return false;
                }
                break;

            default:
                assert(false && "Unhandled operation type.");
                break;
        }
    }

    return true;
}
