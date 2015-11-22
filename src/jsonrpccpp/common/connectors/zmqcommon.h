/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    zmq.h
 * @date    22.11.2015
 * @author  Badaev Vladimir <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/
#ifndef JSONRPC_CPP_ZMQ_H_

#define JSONRPC_CPP_ZMQ_H_

namespace jsonrpc {
    
    /**
     * @brief ZMQCurveKey, class for ZeriMQ CURVE keys. Can store string or binary form.
     */
    class ZMQCurveKey {
            static const std::string curve_punct;
        public:
            static const size_t binary_size = 32;
            static const size_t string_size = 40;

            /**
             * @brief ZMQCurveKey, constructor for the included ZMQCurveKey.
             * @param stringkey, 40-char string, containing ZMQ_CURVE key in string form.
             */
            ZMQCurveKey(const std::string& stringkey);
            /**
             * @brief ZMQCurveKey, constructor for the included ZMQCurveKey.
             * @param binarykey, vector of 32 bytes, containing ZMQ_CURVE key in binary form.
             */
            ZMQCurveKey(const std::vector<unsigned char>& binarykey);
            virtual void ~ZMQCurveKey();

            unsigned char *Key()
            {
                return key.data();
            }
            size_t Size()
            {
                return key.size();
            }
        private:
            std::vector<unsigned char> key;
    }

    /**
     * @brief base class which allows use zmq_setsockopt(3).
     */

    class ZMQEndpointOption {
        public:
            ZMQEndpointOption(const std::string& ep)
                : endpoint(ep)
            {}
            virtual ~ZMQEndpointOption()
            {}

            const char *Endpoint()
            {
                return endpoint.c_str();
            }
            virtual void SocketOptions(zmq::socket_t& s)
            {
            };
        private:
            const std::string endpoint;
    };
    
}; /* namespace jsonrpc */


#endif /* JSONRPC_CPP_ZMQ_H_ */
