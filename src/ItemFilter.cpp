// Copyright (C) 2015 xaizek <xaizek@posteo.net>
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
    return passes([&item](const std::string &f) { return item.getValue(f); });
}

bool
ItemFilter::passes(std::function<std::string(const std::string &)> accessor)
    const
{
    std::string error;
    return passes(accessor, error);
}

bool
ItemFilter::passes(std::function<std::string(const std::string &)> accessor,
                   std::string &error) const
{
    error.clear();

    auto err = [&error](const Cond &cond) {
        if (!error.empty()) {
            error += '\n';
        }
        error += "\tnot met for " + cond.key + ": " + cond.str;
    };

    for (const Cond &cond : conds) {
        const std::string &val = accessor(cond.key);
        switch (cond.op) {
            case Op::eq:
                if (val != cond.value) {
                    err(cond);
                }
                continue;
            case Op::ne:
                if (val == cond.value) {
                    err(cond);
                }
                continue;
            case Op::iccontains:
                if (!boost::icontains(val, cond.value)) {
                    err(cond);
                }
                continue;
            case Op::icnotcontain:
                if (boost::contains(val, cond.value)) {
                    err(cond);
                }
                continue;
        }
        assert(false && "Unhandled operation type.");
    }

    return error.empty();
}
