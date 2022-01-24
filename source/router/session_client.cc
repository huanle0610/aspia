//
// Aspia Project
// Copyright (C) 2020 Dmitry Chapyshev <dmitry@aspia.ru>
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

#include "router/session_client.h"

#include "base/logging.h"
#include "base/crypto/random.h"
#include "base/strings/unicode.h"
#include "router/server.h"
#include "router/session_host.h"
#include "router/session_relay.h"

namespace router {

SessionClient::SessionClient()
    : Session(proto::ROUTER_SESSION_CLIENT)
{
    // Nothing
}

SessionClient::~SessionClient() = default;

void SessionClient::onSessionReady()
{
    // Nothing
}

void SessionClient::onMessageReceived(const base::ByteArray& buffer)
{
    std::unique_ptr<proto::PeerToRouter> message = std::make_unique<proto::PeerToRouter>();
    if (!base::parse(buffer, message.get()))
    {
        LOG(LS_ERROR) << "Could not read message from client";
        return;
    }

    if (message->has_connection_request())
    {
        readConnectionRequest(message->connection_request());
    }
    else if (message->has_check_host_status())
    {
        readCheckHostStatus(message->check_host_status());
    }
    else
    {
        LOG(LS_WARNING) << "Unhandled message from client";
    }
}

void SessionClient::onMessageWritten(size_t /* pending */)
{
    // Nothing
}

void SessionClient::readConnectionRequest(const proto::ConnectionRequest& request)
{
    LOG(LS_INFO) << "New connection request (host_id: " << request.host_id() << ")";

    std::unique_ptr<proto::RouterToPeer> message = std::make_unique<proto::RouterToPeer>();
    proto::ConnectionOffer* offer = message->mutable_connection_offer();

    SessionHost* host = server().hostSessionById(request.host_id());
    if (!host)
    {
        LOG(LS_WARNING) << "Host with id " << request.host_id() << " NOT found!";
        offer->set_error_code(proto::ConnectionOffer::PEER_NOT_FOUND);
    }
    else
    {
        LOG(LS_INFO) << "Host with id " << request.host_id() << " found";

        std::optional<SharedKeyPool::Credentials> credentials = relayKeyPool().takeCredentials();
        if (!credentials.has_value())
        {
            LOG(LS_WARNING) << "Empty key pool";
            offer->set_error_code(proto::ConnectionOffer::KEY_POOL_EMPTY);
        }
        else
        {
            SessionRelay* relay = static_cast<SessionRelay*>(
                server().sessionById(credentials->session_id));
            if (!relay)
            {
                LOG(LS_ERROR) << "No relay with session id " << credentials->session_id;
                offer->set_error_code(proto::ConnectionOffer::KEY_POOL_EMPTY);
            }
            else
            {
                const std::optional<SessionRelay::PeerData>& peer_data = relay->peerData();
                if (!peer_data.has_value())
                {
                    LOG(LS_ERROR) << "No peer data for relay with session id "
                                  << credentials->session_id;
                    offer->set_error_code(proto::ConnectionOffer::KEY_POOL_EMPTY);
                }
                else
                {
                    offer->set_error_code(proto::ConnectionOffer::SUCCESS);

                    proto::RelayCredentials* offer_credentials = offer->mutable_relay();

                    offer_credentials->set_host(relay->peerData()->first);
                    offer_credentials->set_port(relay->peerData()->second);
                    offer_credentials->mutable_key()->Swap(&credentials->key);
                    offer_credentials->set_secret(base::Random::string(16));

                    LOG(LS_INFO) << "Sending connection offer to host";
                    offer->set_peer_role(proto::ConnectionOffer::HOST);
                    host->sendConnectionOffer(*offer);
                }
            }
        }
    }

    LOG(LS_INFO) << "Sending connection offer to client";
    offer->set_peer_role(proto::ConnectionOffer::CLIENT);
    sendMessage(*message);
}

void SessionClient::readCheckHostStatus(const proto::CheckHostStatus& check_host_status)
{
    std::unique_ptr<proto::RouterToPeer> message = std::make_unique<proto::RouterToPeer>();
    proto::HostStatus* host_status = message->mutable_host_status();

    if (server().hostSessionById(check_host_status.host_id()))
        host_status->set_status(proto::HostStatus::STATUS_ONLINE);
    else
        host_status->set_status(proto::HostStatus::STATUS_OFFLINE);

    LOG(LS_INFO) << "Sending host status for host ID " << check_host_status.host_id()
                 << ": " << host_status->status();
    sendMessage(*message);
}

} // namespace router
