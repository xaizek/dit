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
    return passes([&item](const std::string &key) {
        std::vector<std::string> values;
        if (key == "_any") {
            for (const std::string &key : item.listRecordNames()) {
                values.push_back(item.getValue(key));
            }
        } else {
            values.push_back(item.getValue(key));
        }
        return values;
    });
}

bool
ItemFilter::passes(const std::function<accessor_f> &accessor) const
{
    std::string error;
    return passes(accessor, error);
}

bool
ItemFilter::passes(const std::function<accessor_f> &accessor,
                   std::string &error) const
{
    error.clear();

    auto test = [](const Cond &cond, const std::string &val) {
        switch (cond.op) {
            case Op::eq:           return (val == cond.value);
            case Op::ne:           return (val != cond.value);
            case Op::iccontains:   return boost::icontains(val, cond.value);
            case Op::icnotcontain: return !boost::icontains(val, cond.value);
        }
        assert(false && "Unhandled operation type.");
        return false;
    };

    auto err = [&error](const Cond &cond) {
        if (!error.empty()) {
            error += '\n';
        }
        error += "\tnot met for " + cond.key + ": " + cond.str;
    };

    for (const Cond &cond : conds) {
        bool matched = false;
        for (const std::string &val : accessor(cond.key)) {
            if (test(cond, val)) {
                matched = true;
                break;
            }
        }
        if (!matched) {
            err(cond);
        }
    }

    return error.empty();
}
