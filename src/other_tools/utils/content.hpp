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

#ifndef INCLUDED_SRC_OTHER_TOOLS_UTILS_CONTENT_HPP
#define INCLUDED_SRC_OTHER_TOOLS_UTILS_CONTENT_HPP

#include <optional>
#include <string>
#include <variant>

#include "src/buildtool/common/user_structs.hpp"
#include "src/buildtool/crypto/hasher.hpp"
#include "src/buildtool/logging/log_level.hpp"
#include "src/other_tools/just_mr/mirrors.hpp"
#include "src/other_tools/utils/curl_easy_handle.hpp"
#include "src/other_tools/utils/curl_url_handle.hpp"

// Utilities related to the content of an archive

/// \brief Fetches a file from the internet and stores its content in memory.
/// \returns the content.
[[nodiscard]] static auto NetworkFetch(std::string const& fetch_url,
                                       CAInfoPtr const& ca_info) noexcept
    -> std::optional<std::string> {
    auto curl_handle = CurlEasyHandle::Create(
        ca_info->no_ssl_verify, ca_info->ca_bundle, LogLevel::Debug);
    if (not curl_handle) {
        return std::nullopt;
    }
    return curl_handle->DownloadToString(fetch_url);
}

/// \brief Fetches a file from the internet and stores its content in memory.
/// Tries not only a given remote, but also all associated remote locations.
/// \returns An error + data union, with the error message at index 0 and the
/// fetched data at index 1.
[[nodiscard]] static auto NetworkFetchWithMirrors(
    std::string const& fetch_url,
    std::vector<std::string> const& mirrors,
    CAInfoPtr const& ca_info,
    MirrorsPtr const& additional_mirrors) noexcept
    -> std::variant<std::string, std::string> {
    // keep all remotes tried, to report in case fetch fails
    std::string remotes_buffer{};
    // first, try the local mirrors
    std::optional<std::string> data{std::nullopt};
    auto local_mirrors =
        MirrorsUtils::GetLocalMirrors(additional_mirrors, fetch_url);
    for (auto const& mirror : local_mirrors) {
        if (data = NetworkFetch(mirror, ca_info); data) {
            break;
        }
        // add local mirror to buffer
        remotes_buffer.append(fmt::format("\n> {}", mirror));
    }
    if (not data) {
        // get preferred hostnames list
        auto preferred_hostnames =
            MirrorsUtils::GetPreferredHostnames(additional_mirrors);
        // try the main fetch URL, but with each of the preferred hostnames
        for (auto const& hostname : preferred_hostnames) {
            if (auto preferred_url =
                    CurlURLHandle::ReplaceHostname(fetch_url, hostname)) {
                if (data = NetworkFetch(*preferred_url, ca_info); data) {
                    break;
                }
                // add preferred URL to buffer
                remotes_buffer.append(fmt::format("\n> {}", *preferred_url));
            }
            else {
                // report failed hostname
                remotes_buffer.append(
                    fmt::format("\n> {} (failed hostname replace: {})",
                                fetch_url,
                                hostname));
            }
        }
        if (not data) {
            // now try the main fetch URL
            if (data = NetworkFetch(fetch_url, ca_info); not data) {
                // add main fetch URL to buffer
                remotes_buffer.append(fmt::format("\n> {}", fetch_url));
                // try the mirrors, in order, if given
                for (auto const& mirror : mirrors) {
                    // first use with preferred hostnames...
                    for (auto const& hostname : preferred_hostnames) {
                        if (auto preferred_mirror =
                                CurlURLHandle::ReplaceHostname(mirror,
                                                               hostname)) {
                            if (data = NetworkFetch(*preferred_mirror, ca_info);
                                data) {
                                break;
                            }
                            // add preferred mirror to buffer
                            remotes_buffer.append(
                                fmt::format("\n> {}", *preferred_mirror));
                        }
                        else {
                            // report failed hostname
                            remotes_buffer.append(fmt::format(
                                "\n> {} (failed hostname replace: {})",
                                mirror,
                                hostname));
                        }
                    }
                    // ...then the original mirror
                    if (not data) {
                        if (data = NetworkFetch(mirror, ca_info); data) {
                            break;
                        }
                        // add mirror to buffer
                        remotes_buffer.append(fmt::format("\n> {}", mirror));
                    }
                }
            }
        }
    }
    return data ? std::variant<std::string, std::string>(std::in_place_index<1>,
                                                         *data)
                : std::variant<std::string, std::string>(std::in_place_index<0>,
                                                         remotes_buffer);
}

template <Hasher::HashType type>
[[nodiscard]] static auto GetContentHash(std::string const& data) noexcept
    -> std::string {
    Hasher hasher{type};
    hasher.Update(data);
    auto digest = std::move(hasher).Finalize();
    return digest.HexString();
}

#endif  // INCLUDED_SRC_OTHER_TOOLS_UTILS_CONTENT_HPP
