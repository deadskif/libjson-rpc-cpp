/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    zmqclient.h
 * @date    17.11.2015
 * @author  Badaev Vladimir <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/
#ifndef JSONRPC_CPP_ZMQCLIENT_H_

#define JSONRPC_CPP_ZMQCLIENT_H_
#include <string>
#include <vector> /* for ZMQCurveKey */
#include <cctype> /* for ZMQCurveKey */
#include <zmq.hpp>

#include "../iclientconnector.h"
#include <jsonrpccpp/common/exception.h>
#include <jsonrpccpp/common/connectors/zmqcommon.h>

namespace jsonrpc {
    /**
     * @brief ZMQClientCurve, class for client CURVE encryption support.
     */
    class ZMQClientCurve : public ZMQEndpointOption {
        public:
            /**
             * @brief ZMQClientCurve, constructor for the included ZMQClientCurve.
             * @param endpoint, string containing zeromq endpoint.
             * @param serverkey, ZMQCurveKey containing ZMQ_CURVE_SERVERKEY socket option.
             * @param publickey, ZMQCurveKey containing ZMQ_CURVE_PUBLICKEY socket option.
             * @param secretkey, ZMQCurveKey containing ZMQ_CURVE_SECRETKEY socket option.
             * @see http://api.zeromq.org/4-0:zmq-curve 
             */
            ZMQClientCurve(const std::string& endpoint,
                    const ZMQCurveKey& serverkey,
                    const ZMQCurveKey& publickey,
                    const ZMQCurveKey& secretkey);
            virtual ~ZMQClientCurve;

            virtual void SocketOptions(zmq::socket_t& s);
        private:
            ZMQCurveKey serverkey;
            ZMQCurveKey publickey;
            ZMQCurveKey secretkey;
    };
    class ZMQClient : public IClientConnector
    {
        public:
            /**
             * @brief ZMQClient, constructor for the included ZMQClient.
             * @param endpoint, string containing zeromq endpoint.
             */
            ZMQClient(const std::string& endpoint);
            /**
             * @brief ZMQClient, constructor for the included ZMQClient.
             * @param endpoint, ZMQEndpointOption containing zeromq endpoint and zmq_socket options.
             */
            ZMQClient(const ZMQEndpointOption& endpoint);
            virtual ~ZMQClient();
            virtual void SendRPCMessage(const std::string& message, std::string& result) throw (JsonRpcException);


        private:
            zmq::context_t ctx;
            zmq::socket_t sock;
    };
    
} /* jsonrpc */


#endif /* end of include guard: JSONRPC_CPP_ZMQCLIENT_H_ */
