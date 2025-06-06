//
// Aspia Project
// Copyright (C) 2016-2025 Dmitry Chapyshev <dmitry@aspia.ru>
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
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef BASE_MEMORY_SERIALIZER_H
#define BASE_MEMORY_SERIALIZER_H

#include "base/macros_magic.h"
#include "base/memory/byte_array.h"

#include <array>

namespace base {

class Serializer
{
public:
    Serializer();
    ~Serializer();

    static constexpr size_t kPoolSize = 32;
    static constexpr size_t kBufferSize = 1024;

    base::ByteArray serialize(const google::protobuf::MessageLite& message);
    void addBuffer(base::ByteArray&& buffer);

private:
    std::array<base::ByteArray, kPoolSize> pool_;
    size_t free_ = kPoolSize;

    DISALLOW_COPY_AND_ASSIGN(Serializer);
};

} // namespace base

#endif // BASE_MEMORY_SERIALIZER_H
