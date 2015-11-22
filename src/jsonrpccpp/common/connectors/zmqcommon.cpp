/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    zmq.cpp
 * @date    22.11.2015
 * @author  Badaev Vladimir <dead.skif@gmail.com>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "zmqcommon.h"
using namespace std;
using namespace jsonrpc;
using namespace zmq;

/**
 * @brief curve_punct Got it from https://github.com/zeromq/libcurve/blob/master/src/curve_z85.c
 */
const string ZMQCurveKey::curve_punct = ".-:+=^!/*?&<>()[]{}@%$#";
ZMQCurveKey::ZMQCurveKey(const std::string& stringkey)
    : key(string_size+1);
{
    if (stringkey.size() != string_size)
        throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, "Bad key size");
    for (const auto& c: stringkey) {
        if (!isalnum(c) && (curve_punct.find(c) == curve_punct.end()))
            throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, "Bad key char");
    }
    strcpy(key.data(), stringkey.c_str());
}
ZMQCurveKey::ZMQCurveKey(const std::vector<unsigned char>& binarykey)
    : key(binarykey)
{
    if (binarykey.size() != binary_size)
        throw JsonRpcException(Errors::ERROR_CLIENT_CONNECTOR, "Bad key size");
}
~ZMQCurveKey::ZMQCurveKey()
{}
