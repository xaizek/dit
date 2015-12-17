// Copyright (C) 2015 xaizek <xaizek@openmailbox.org>
//
// This file is part of dit.
//
// dit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dit.  If not, see <http://www.gnu.org/licenses/>.

#include "ItemFilter.hpp"

#include <cassert>

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>

#include "Item.hpp"
#include "parsing.hpp"

ItemFilter::ItemFilter(const std::vector<std::string> &exprs)
{
    for (const std::string &expr : exprs) {
        Cond cond;
        auto iter = expr.cbegin();
        if (!parseCond(iter, expr.cend(), cond)) {
            throw std::runtime_error("Wrong expression: " + expr);
        }
        conds.emplace_back(std::move(cond));
    }
}

ItemFilter::ItemFilter(Cond cond)
{
    conds.emplace_back(std::move(cond));
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
            case Op::iccontains:
                if (!boost::icontains(val, cond.value)) {
                    return false;
                }
                break;
            case Op::icnotcontain:
                if (boost::contains(val, cond.value)) {
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
