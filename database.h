#pragma once

#include <algorithm>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

enum class DataType
{
    INT,
    STRING
};

using Value = std::variant<int, std::string>;

struct Column
{
    std::string name;
    DataType type;
};

class Schema
{
public:
    explicit Schema(std::vector<Column> columns)
        : columns_(std::move(columns))
    {
        std::set<std::string> names;
        for (const Column &column : columns_)
        {
            if (column.name.empty())
            {
                throw std::invalid_argument("Column names cannot be empty");
            }
            if (!names.insert(column.name).second)
            {
                throw std::invalid_argument("Column names must be unique");
            }
            if (column.type != DataType::INT && column.type != DataType::STRING)
            {
                throw std::invalid_argument("Column has an unsupported data type");
            }
        }
    }

    const std::vector<Column> &columns() const noexcept
    {
        return columns_;
    }

    void validate(const std::vector<Value> &values) const
    {
        if (values.size() != columns_.size())
        {
            throw std::invalid_argument("Row value count does not match schema");
        }

        for (std::size_t index = 0; index < columns_.size(); ++index)
        {
            const bool matches = columns_[index].type == DataType::INT
                                     ? std::holds_alternative<int>(values[index])
                                     : std::holds_alternative<std::string>(values[index]);
            if (!matches)
            {
                throw std::invalid_argument(
                    "Value type does not match column: " + columns_[index].name);
            }
        }
    }

private:
    std::vector<Column> columns_;
};

class Row
{
public:
    using Id = std::size_t;

    Id id() const noexcept
    {
        return id_;
    }

    const std::vector<Value> &values() const noexcept
    {
        return values_;
    }

private:
    friend class Table;

    Row(Id id, std::vector<Value> values)
        : id_(id), values_(std::move(values))
    {
    }

    Id id_;
    std::vector<Value> values_;
};

class Table
{
public:
    Table(std::string name, Schema schema)
        : name_(std::move(name)), schema_(std::move(schema))
    {
        if (name_.empty())
        {
            throw std::invalid_argument("Table name cannot be empty");
        }
    }

    const std::string &name() const noexcept
    {
        return name_;
    }

    const Schema &schema() const noexcept
    {
        return schema_;
    }

    Row::Id insertRow(std::vector<Value> values)
    {
        schema_.validate(values);
        const Row::Id id = nextRowId_;
        rows_.push_back(Row(id, std::move(values)));
        ++nextRowId_;
        return id;
    }

    const std::vector<Row> &readAll() const noexcept
    {
        return rows_;
    }

    bool deleteRow(Row::Id id)
    {
        const auto row = std::find_if(rows_.begin(), rows_.end(),
                                      [id](const Row &candidate)
                                      { return candidate.id() == id; });
        if (row == rows_.end())
        {
            return false;
        }

        rows_.erase(row);
        return true;
    }

private:
    std::string name_;
    Schema schema_;
    std::vector<Row> rows_;
    Row::Id nextRowId_ = 1;
};

class Database
{
public:
    Table &createTable(std::string name, Schema schema)
    {
        if (std::any_of(tables_.begin(), tables_.end(),
                        [&name](const Table &table)
                        { return table.name() == name; }))
        {
            throw std::invalid_argument("A table with this name already exists");
        }

        tables_.emplace_back(std::move(name), std::move(schema));
        return tables_.back();
    }

    Table &getTable(const std::string &name)
    {
        const auto table = std::find_if(tables_.begin(), tables_.end(),
                                        [&name](const Table &candidate)
                                        { return candidate.name() == name; });
        if (table == tables_.end())
        {
            throw std::out_of_range("Table not found: " + name);
        }
        return *table;
    }

    const Table &getTable(const std::string &name) const
    {
        const auto table = std::find_if(tables_.begin(), tables_.end(),
                                        [&name](const Table &candidate)
                                        { return candidate.name() == name; });
        if (table == tables_.end())
        {
            throw std::out_of_range("Table not found: " + name);
        }
        return *table;
    }

    const std::vector<Table> &tables() const noexcept
    {
        return tables_;
    }

private:
    std::vector<Table> tables_;
};
