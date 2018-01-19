//
// PROJECT:         Aspia
// FILE:            report/report_json_file.cc
// LICENSE:         Mozilla Public License Version 2.0
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "base/logging.h"
#include "report/report_json_file.h"

namespace aspia {

ReportJsonFile::ReportJsonFile(std::ofstream file)
    : writer_(buffer_),
      file_(std::move(file))
{
    writer_.SetIndent('\t', 1);
}

// static
std::unique_ptr<ReportJsonFile> ReportJsonFile::Create(
    const std::experimental::filesystem::path& file_path)
{
    std::ofstream file;

    file.open(file_path, std::ofstream::out | std::ofstream::trunc);
    if (!file.is_open())
    {
        LOG(WARNING) << "Unable to create report file: " << file_path;
        return nullptr;
    }

    return std::unique_ptr<ReportJsonFile>(new ReportJsonFile(std::move(file)));
}

void ReportJsonFile::StartDocument()
{
    writer_.StartObject();
}

void ReportJsonFile::EndDocument()
{
    writer_.EndObject();
    file_.write(buffer_.GetString(), buffer_.GetSize());
    file_.close();
}

void ReportJsonFile::StartTableGroup(std::string_view name)
{
    writer_.String(name.data());
    writer_.StartObject();
}

void ReportJsonFile::EndTableGroup()
{
    writer_.EndObject();
}

void ReportJsonFile::StartTable(Category* category)
{
    DCHECK(!category_);
    category_ = category;
    DCHECK(category_);

    writer_.String(category->Name());
    writer_.StartObject();

    if (category_->type() == Category::Type::INFO_LIST)
        writer_.StartArray();
}

void ReportJsonFile::EndTable()
{
    DCHECK(category_);

    if (category_->type() == Category::Type::INFO_LIST)
        writer_.EndArray();

    writer_.EndObject();
    column_list_.clear();

    category_ = nullptr;
}

void ReportJsonFile::AddColumns(const ColumnList& column_list)
{
    for (const auto& column : column_list)
    {
        column_list_.emplace_back(column.first);
    }
}

void ReportJsonFile::StartGroup(std::string_view name)
{
    writer_.String(name.data());
    writer_.StartObject();
}

void ReportJsonFile::EndGroup()
{
    writer_.EndObject();
}

void ReportJsonFile::AddParam(std::string_view param, const Value& value)
{
    if (!value.HasUnit())
    {
        writer_.Key(param.data());
        WriteValue(value);
    }
    else
    {
        writer_.String(param.data());
        writer_.StartObject();

        writer_.Key("value");
        WriteValue(value);

        writer_.Key("unit");
        writer_.String(value.Unit().data());

        writer_.EndObject();
    }
}

void ReportJsonFile::StartRow()
{
    writer_.StartObject();
    column_index_ = 0;
}

void ReportJsonFile::EndRow()
{
    writer_.EndObject();
}

void ReportJsonFile::AddValue(const Value& value)
{
    if (!value.HasUnit())
    {
        writer_.Key(column_list_[column_index_].data());
        WriteValue(value);
    }
    else
    {
        writer_.String(column_list_[column_index_].data());
        writer_.StartObject();

        writer_.Key("value");
        WriteValue(value);

        writer_.Key("unit");
        writer_.String(value.Unit().data());

        writer_.EndObject();
    }

    ++column_index_;
}

void ReportJsonFile::WriteValue(const Value& value)
{
    switch (value.type())
    {
        case Value::Type::STRING:
            writer_.String(value.ToString().data());
            break;

        case Value::Type::BOOL:
            writer_.Bool(value.ToBool());
            break;

        case Value::Type::UINT32:
            writer_.Uint(value.ToUint32());
            break;

        case Value::Type::INT32:
            writer_.Int(value.ToInt32());
            break;

        case Value::Type::UINT64:
            writer_.Uint64(value.ToUint64());
            break;

        case Value::Type::INT64:
            writer_.Int64(value.ToInt64());
            break;

        case Value::Type::DOUBLE:
            writer_.Double(value.ToDouble());
            break;

        case Value::Type::MEMORY_SIZE:
            writer_.Double(value.ToMemorySize());
            break;

        default:
            DLOG(FATAL) << "Unhandled value type: " << static_cast<int>(value.type());
            break;
    }
}

} // namespace aspia
