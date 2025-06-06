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

#ifndef CLIENT_UI_APPLICATION_H
#define CLIENT_UI_APPLICATION_H

#include "qt_base/application.h"

namespace client {

class Application final : public qt_base::Application
{
    Q_OBJECT

public:
    Application(int& argc, char* argv[]);
    virtual ~Application() final;

    static Application* instance();

private:
    DISALLOW_COPY_AND_ASSIGN(Application);
};

} // namespace host

#endif // CLIENT_UI_APPLICATION_H
