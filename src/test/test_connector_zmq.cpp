/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    test_connector_zmq.cpp
 * @date    17/11/2015
 * @author  Vladimir Badaev <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifdef ZMQCONNECTORS_TESTING
#include <unistd.h>

#include <catch.hpp>
#include <jsonrpccpp/server/connectors/zmqserver.h>
#include <jsonrpccpp/client/connectors/zmqclient.h>
#include "mockclientconnectionhandler.h"

#include "checkexception.h"


using namespace jsonrpc;
using namespace std;

#define TEST_MODULE "[connector_zmq]"

#define IPC_SOCKET_PATH "/tmp/jsonrpccpp-zmq-socket"
#define IPC_ENDPOINT "ipc://" IPC_SOCKET_PATH
#define TCP_ENDPOINT "tcp://127.0.0.1:9999"

namespace testzmqserver
{
    struct F {
            ZMQServer server;
            ZMQClient ipc_client;
            ZMQClient tcp_client;
            MockClientConnectionHandler handler;

            F(unsigned int th=0) :
                server(vector<string>({ IPC_ENDPOINT, TCP_ENDPOINT }), th),
                ipc_client(IPC_ENDPOINT),
                tcp_client(TCP_ENDPOINT)
            {
                server.SetHandler(&handler);
                REQUIRE(server.StartListening());
            }
            virtual ~F()
            {
                server.StopListening();
                unlink(IPC_SOCKET_PATH);
            }
    };

    struct FTH : public F {
        FTH() : F(2) {};
        virtual ~FTH() {};
    };

#define ZMQCURVE_CLIENT_PUBLIC_KEY "Yne@$w-vo<fVvi]a<NY6T1ed:M$fCG*[IaLV{hID"
#define ZMQCURVE_CLIENT_SECRET_KEY "D:)Q[IlAW!ahhC2ac:9*A}h:p?([4%wOTJ%JR%cs"
#define ZMQCURVE_SERVER_PUBLIC_KEY "rq:rM>}U?@Lns47E1%kR.o@n%FcmmsL/@{H8]yf7"
#define ZMQCURVE_SERVER_SECRET_KEY "JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6"
    struct CF {
        ZMQServer server;
        ZMQClient ipc_client;
        ZMQClient tcp_client;
        MockClientConnectionHandler handler;
        CF(unsigned int th=0)
            : server(std::move([] {
                        ZMQEndpointOption::Vector v;
                        v.push_back(ZMQServerCurve::make(IPC_ENDPOINT, ZMQCURVE_SERVER_SECRET_KEY));
                        v.push_back(ZMQServerCurve::make(TCP_ENDPOINT, ZMQCURVE_SERVER_SECRET_KEY));
                        return std::move(v);
                    }()
                ), th),
            ipc_client(ZMQClientCurve::make(IPC_ENDPOINT, ZMQCURVE_SERVER_PUBLIC_KEY, ZMQCURVE_CLIENT_PUBLIC_KEY, ZMQCURVE_CLIENT_SECRET_KEY)),
            tcp_client(ZMQClientCurve::make(TCP_ENDPOINT, ZMQCURVE_SERVER_PUBLIC_KEY, ZMQCURVE_CLIENT_PUBLIC_KEY, ZMQCURVE_CLIENT_SECRET_KEY))
        {
            server.SetHandler(&handler);
            REQUIRE(server.StartListening());
        }
        virtual ~CF()
        {
            server.StopListening();
            unlink(IPC_SOCKET_PATH);
        }
    };



    bool check_exception1(JsonRpcException const&ex)
    {
        return ex.GetCode() == Errors::ERROR_CLIENT_CONNECTOR;
    }

    bool check_exception2(JsonRpcException const&ex)
    {
        return ex.GetCode() == Errors::ERROR_RPC_INTERNAL_ERROR;
    }
};
using namespace testzmqserver;


TEST_CASE_METHOD(F, "test_zmq_ipc_success", TEST_MODULE)
{
    handler.response = "exampleresponse_ipc";
    string result;
    ipc_client.SendRPCMessage("examplerequest_ipc", result);

    CHECK(handler.request == "examplerequest_ipc");
    CHECK(result == "exampleresponse_ipc");
}

TEST_CASE_METHOD(F, "test_zmq_tcp_success", TEST_MODULE)
{
    handler.response = "exampleresponse_tcp";
    string result;
    tcp_client.SendRPCMessage("examplerequest_tcp", result);

    CHECK(handler.request == "examplerequest_tcp");
    CHECK(result == "exampleresponse_tcp");
}

TEST_CASE_METHOD(FTH, "test_zmq_threaded_ipc_success", TEST_MODULE)
{
    handler.response = "exampleresponse_ipc_th";
    string result;
    ipc_client.SendRPCMessage("examplerequest_ipc_th", result);

    CHECK(handler.request == "examplerequest_ipc_th");
    CHECK(result == "exampleresponse_ipc_th");
}

TEST_CASE_METHOD(FTH, "test_zmq_threaded_tcp_success", TEST_MODULE)
{
    handler.response = "exampleresponse_tcp_th";
    string result;
    tcp_client.SendRPCMessage("examplerequest_tcp_th", result);

    CHECK(handler.request == "examplerequest_tcp_th");
    CHECK(result == "exampleresponse_tcp_th");
}
TEST_CASE_METHOD(CF, "test_zmq_curve_ipc_success", TEST_MODULE)
{
    handler.response = "exampleresponse_ipc_cu";
    string result;
    ipc_client.SendRPCMessage("examplerequest_ipc_cu", result);

    CHECK(handler.request == "examplerequest_ipc_cu");
    CHECK(result == "exampleresponse_ipc_cu");
}

TEST_CASE_METHOD(CF, "test_zmq_curve_tcp_success", TEST_MODULE)
{
    handler.response = "exampleresponse_tcp_cu";
    string result;
    tcp_client.SendRPCMessage("examplerequest_tcp_cu", result);

    CHECK(handler.request == "examplerequest_tcp_cu");
    CHECK(result == "exampleresponse_tcp_cu");
}

TEST_CASE("test_zmq_client_invalid", TEST_MODULE)
{
    CHECK_EXCEPTION_TYPE(ZMQClient client("ddd://bad"), JsonRpcException, check_exception1);
}
TEST_CASE("test_zmq_server_invalid", TEST_MODULE)
{
    ZMQServer server("ddd://bad");
    CHECK(server.StartListening() == false);
}

TEST_CASE("test_zmq_server_multiplestart", TEST_MODULE)
{
    /* ZeroMQ can binds multiply on ipc:// (aka unix sockets) */
    ZMQServer server(TCP_ENDPOINT);
    CHECK(server.StartListening() == true);
    CHECK(server.StartListening() == false);

    ZMQServer server2(TCP_ENDPOINT);
    CHECK(server2.StartListening() == false);
    CHECK(server2.StopListening() == false);

    CHECK(server.StopListening() == true);

    unlink(IPC_ENDPOINT);
}

#if 0
TEST_CASE("test_unixdomainsocket_client_invalid", TEST_MODULE)
{
    UnixDomainSocketClient client("tmp/someinvalidpath");
    string result;
    CHECK_EXCEPTION_TYPE(client.SendRPCMessage("foobar", result), JsonRpcException, check_exception1);
}
#endif
#endif
