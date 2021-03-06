// Optima is a C++ library for solving linear and non-linear constrained optimization problems
//
// Copyright (C) 2020 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "Outputter.hpp"

// Optima includes
#include <Optima/Exception.hpp>

namespace Optima {
namespace {

/// Return the width for std::setw to be used in Outputter
auto colwidth(std::size_t width, const std::string& str) -> std::size_t
{
    return width > str.size() + 4 ? width : str.size() + 4;
}

/// Return the bar string to be used in Outputter
auto barstr(std::size_t width, const std::string& str) -> std::string
{
    return std::string(colwidth(width, str), '=');
}

} // namespace

Outputter::Outputter()
{}

void Outputter::clear()
{
	values.clear();
	entries.clear();
}

void Outputter::setOptions(const OutputterOptions& options_)
{
    options = options_;
}

void Outputter::addEntry(const std::string& name)
{
    entries.push_back(name);
}

void Outputter::addEntries(const std::string& prefix, std::size_t size)
{
    for(std::size_t i = 0; i < size; ++i)
    {
        std::stringstream ss;
        ss << prefix << "[" << i << "]";
        addEntry(ss.str());
    }
}

void Outputter::addEntries(const std::string& prefix, std::size_t size, const std::vector<std::string>& names)
{
    errorif(names.size() != 0 && names.size() != size, "Mismatched number of "
        "names given for variables with prefix `", prefix, "`. "
        "It should have been ", size, ", but got ", names.size(), " instead.");

    if(names.empty())
        addEntries(prefix, size);
    else
    {
        for(std::string name : names)
        {
            std::stringstream ss;
            ss << prefix << "[" << name << "]";
            addEntry(ss.str());
        }
    }
}

void Outputter::addEntrySeparator()
{
    entries.push_back(options.separator);
}

void Outputter::addValueSeparator()
{
    values.push_back(options.separator);
}

void Outputter::outputHeader()
{
    if(options.active)
    {
        for(const std::string& entry : entries)
            std::cout << (entry == options.separator ? options.separator : barstr(options.width, entry));
        std::cout << std::endl;

        for(const std::string& entry : entries)
            if(entry == options.separator) std::cout << options.separator;
            else std::cout << std::setw(colwidth(options.width, entry)) << std::left << entry;
        std::cout << std::endl;

        for(const std::string& entry : entries)
            std::cout << (entry == options.separator ? options.separator : barstr(options.width, entry));
        std::cout << std::endl;
    }
}

void Outputter::outputState()
{
    if(options.active)
    {
        assert(entries.size() == values.size());
        auto entry = entries.begin();
        for(const std::string& val : values)
            if(val == options.separator) std::cout << options.separator;
            else std::cout << std::setw(colwidth(options.width, *entry++)) << std::left << val;
        std::cout << std::endl;

        values.clear();
    }
}

void Outputter::outputMessage(const std::string& message)
{
    if(options.active) std::cout << message << std::endl;
}

} // namespace
