/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    zmqclient.cpp
 * @date    17.11.2015
 * @author  Badaev Vladimir <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/
#include "zmqclient.h"

using namespace std;
using namespace jsonrpc;
using namespace zmq;

#define REQUEST_TIMEOUT     2500    //  msecs, (> 1000!)

ZMQClientCurve::ZMQClientCurve(const std::string& ep,
                    const ZMQCurveKey& serkey,
                    const ZMQCurveKey& pubkey,
                    const ZMQCurveKey& seckey)
    : ZMQEndpointOption(ep),
    serverkey(serkey),
    publickey(pubkey),
    secretkey(seckey)
{
}
void ZMQClientCurve::SocketOptions(zmq::socket_t& s)
{

    s.setsockopt(ZMQ_CURVE_SERVERKEY, serverkey.Key(), serverkey.Size());
    s.setsockopt(ZMQ_CURVE_PUBLICKEY, publickey.Key(), publickey.Size());
    s.setsockopt(ZMQ_CURVE_SECRETKEY, secretkey.Key(), secretkey.Size());

}
ZMQClient::ZMQClient(const std::string& endpoint)
    : ZMQClient(makeZMQEndpointOption<ZMQEndpointOption>(endpoint))
{
}
ZMQClient::ZMQClient(ZMQEndpointOption::Ptr endpoint)
    : ctx(), sock(ctx, ZMQ_REQ)
{
    try {
        endpoint->SocketOptions(sock);
        sock.connect(endpoint->Endpoint());
        const int linger = 0;
        sock.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    } catch (const zmq::error_t& e) {
        throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, "Could not connect to " + std::string(endpoint->Endpoint()) + ":" + e.what());
    }
}
ZMQClient::~ZMQClient()
{}
void ZMQClient::SendRPCMessage(const std::string& message, std::string& result) throw (JsonRpcException)
{
    try {
        sock.send(message.c_str(), message.size());
        pollitem_t items[] = { { sock, 0, ZMQ_POLLIN, 0 } };
        poll (&items[0], 1, REQUEST_TIMEOUT);

        //  If we got a reply, process it
        if (items[0].revents & ZMQ_POLLIN) {
            message_t msg;
            sock.recv(&msg);
            string s(static_cast<const char *>(msg.data()), msg.size());
            result = s;
        } else {
            throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, "Timeout");
        }
    } catch (const zmq::error_t& e) {
        throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, e.what());
    }
}
