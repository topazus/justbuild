// Copyright 2022 Huawei Cloud Computing Technology Co., Ltd.
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

#include "catch2/catch_session.hpp"
#include "src/buildtool/file_system/git_context.hpp"
#include "test/utils/logging/log_config.hpp"

auto main(int argc, char* argv[]) -> int {
    ConfigureLogging();

    /**
     * The current implementation of libgit2 uses pthread_key_t incorrectly
     * on POSIX systems to handle thread-specific data, which requires us to
     * explicitly make sure the main thread is the first one to call
     * git_libgit2_init. Future versions of libgit2 will hopefully fix this.
     */
    GitContext::Create();

    return Catch::Session().run(argc, argv);
}
