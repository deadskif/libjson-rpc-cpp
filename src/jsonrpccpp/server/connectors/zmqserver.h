/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    zmqserver.h
 * @date    15.11.2015
 * @author  Badaev Vladimir <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/

#ifndef JSONRPC_CPP_ZMQSERVER_H_

#define JSONRPC_CPP_ZMQSERVER_H_
#include <vector>
#include <string>
#include <thread>

#include <zmq.hpp>
#include "../abstractserverconnector.h"
#include <jsonrpccpp/common/connectors/zmq.h>

namespace jsonrpc
{
    class ZMQServerCurve : public ZMQEndpointOption {
        public:
            /**
             * @brief ZMQServerCurve, constructor for the included ZMQServerCurve.
             * @param endpoint, string containing zeromq endpoint.
             * @param secretkey, ZMQCurveKey containing ZMQ_CURVE_SECRETKEY socket option.
             * @see http://api.zeromq.org/4-0:zmq-curve 
             */
            ZMQClientCurve(const std::string& endpoint,
                    const ZMQCurveKey& secretkey);
            virtual ~ZMQClientCurve;

            virtual void SocketOptions(zmq::socket_t& s);
        private:
            ZMQCurveKey secretkey;
    };
    class ZMQListener;
    /**
     * This class provides ZeroMQ REP/REQ Socket Server,to handle incoming Requests.
     */
    class ZMQServer: public AbstractServerConnector
    {
        public:

            /**
             * @brief ZeroMQSocketServer, constructor for the included ZeroMQSocketServer
             * @param endpoint, a string containing the ZeroMQ endpoint or IP address.
             * @param threads_count, 0 for 1-thread variant, else will run threads_count threads for each endpoint.
             */
            ZMQServer(const std::string& endpoint="*", unsigned int threads_coun=0);
            /**
             * @brief ZeroMQSocketServer, constructor for the included ZeroMQSocketServer
             * @param endpoints, a vector of strings containing the ZeroMQ endpoints.
             * @param threads_count, 0 for 1-thread variant, else will run threads_count threads for each endpoint.
             */
            ZMQServer(std::vector<std::string> endpoints, unsigned int threads_count=0);

            virtual ~ZMQServer();

            virtual bool StartListening();
            /**
             * @brief StopListening, due to no simple way to stop c11 std::thread, multithreaded ZMQServer stops for a LONG time.
             */
            virtual bool StopListening();

            bool virtual SendResponse(const std::string& response, void* addInfo = NULL);


        protected:
            std::vector<std::string> endpoints;
            std::unique_ptr<ZMQListener> listener;
            unsigned int threads_count;

    };
} /* namespace jsonrpc */
#endif /* JSONRPC_CPP_ZMQSERVER_H_ */
