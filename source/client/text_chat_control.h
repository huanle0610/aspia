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

#ifndef CLIENT_TEXT_CHAT_CONTROL_H
#define CLIENT_TEXT_CHAT_CONTROL_H

namespace proto {
class TextChat;
} // namespace proto

namespace client {

class TextChatControl
{
public:
    virtual ~TextChatControl() = default;

    virtual void onTextChatMessage(const proto::TextChat& message) = 0;
};

} // namespace client

#endif // CLIENT_TEXT_CHAT_CONTROL_H
