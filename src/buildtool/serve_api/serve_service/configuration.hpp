// Copyright 2023 Huawei Cloud Computing Technology Co., Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SRC_BUILD_SERVE_API_SERVE_SERVICE_CONFIGURATION_HPP
#define INCLUDED_SRC_BUILD_SERVE_API_SERVE_SERVICE_CONFIGURATION_HPP

#include "justbuild/just_serve/just_serve.grpc.pb.h"

class ConfigurationService final
    : public justbuild::just_serve::Configuration::Service {
  public:
    // Returns the address of the associated remote endpoint, if set,
    // or an empty string signaling that the serve endpoint acts also
    // as a remote execution endpoint.
    //
    // There are no method-specific errors.
    auto RemoteExecutionEndpoint(
        ::grpc::ServerContext* context,
        const ::justbuild::just_serve::RemoteExecutionEndpointRequest* request,
        ::justbuild::just_serve::RemoteExecutionEndpointResponse* response)
        -> ::grpc::Status override;
};

#endif  // INCLUDED_SRC_BUILD_SERVE_API_SERVE_SERVICE_CONFIGURATION_HPP
