// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/status_code.hpp"

namespace co_http_uring {

std::string_view status_code_message(StatusCode code) {
    switch (code) {
    case StatusCode::CONTINUE: return "Continue";
    case StatusCode::SWITCHING_PROTOCOLS: return "Switching Protocols";
    case StatusCode::PROCESSING: return "Processing";
    case StatusCode::EARLY_HINTS: return "Early Hints";
    case StatusCode::OK: return "OK";
    case StatusCode::CREATED: return "Created";
    case StatusCode::ACCEPTED: return "Accepted";
    case StatusCode::NON_AUTHORITATIVE_INFORMATION:
        return "Non-Authoritative Information";
    case StatusCode::NO_CONTENT: return "No Content";
    case StatusCode::RESET_CONTENT: return "Reset Content";
    case StatusCode::PARTIAL_CONTENT: return "Partial Content";
    case StatusCode::MULTI_STATUS: return "Multi-Status";
    case StatusCode::ALREADY_REPORTED: return "Already Reported";
    case StatusCode::IM_USED: return "IM Used";
    case StatusCode::MULTIPLE_CHOICES: return "Multiple Choices";
    case StatusCode::MOVED_PERMANENTLY: return "Moved Permanently";
    case StatusCode::FOUND: return "Found";
    case StatusCode::SEE_OTHER: return "See Other";
    case StatusCode::NOT_MODIFIED: return "Not Modified";
    case StatusCode::USE_PROXY: return "Use Proxy";
    case StatusCode::TEMPORARY_REDIRECT: return "Temporary Redirect";
    case StatusCode::PERMANENT_REDIRECT: return "Permanent Redirect";
    case StatusCode::BAD_REQUEST: return "Bad Request";
    case StatusCode::UNAUTHORIZED: return "Unauthorized";
    case StatusCode::PAYMENT_REQUIRED: return "Payment Required";
    case StatusCode::FORBIDDEN: return "Forbidden";
    case StatusCode::NOT_FOUND: return "Not Found";
    case StatusCode::METHOD_NOT_ALLOWED: return "Method Not Allowed";
    case StatusCode::NOT_ACCEPTABLE: return "Not Acceptable";
    case StatusCode::PROXY_AUTHENTICATION_REQUIRED:
        return "Proxy Authentication Required";
    case StatusCode::REQUEST_TIMEOUT: return "Request Timeout";
    case StatusCode::CONFLICT: return "Conflict";
    case StatusCode::GONE: return "Gone";
    case StatusCode::LENGTH_REQUIRED: return "Length Required";
    case StatusCode::PRECONDITION_FAILED: return "Precondition Failed";
    case StatusCode::CONTENT_TOO_LARGE: return "Content Too Large";
    case StatusCode::URI_TOO_LONG: return "URI Too Long";
    case StatusCode::UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
    case StatusCode::RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
    case StatusCode::EXPECTATION_FAILED: return "Expectation Failed";
    case StatusCode::MISDIRECTED_REQUEST: return "Misdirected Request";
    case StatusCode::UNPROCESSABLE_CONTENT: return "Unprocessable Content";
    case StatusCode::LOCKED: return "Locked";
    case StatusCode::FAILED_DEPENDENCY: return "Failed Dependency";
    case StatusCode::TOO_EARLY: return "Too Early";
    case StatusCode::UPGRADE_REQUIRED: return "Upgrade Required";
    case StatusCode::PRECONDITION_REQUIRED: return "Precondition Required";
    case StatusCode::TOO_MANY_REQUESTS: return "Too Many Requests";
    case StatusCode::REQUEST_HEADER_FIELDS_TOO_LARGE:
        return "Request Header Fields Too Large";
    case StatusCode::UNAVAILABLE_FOR_LEGAL_REASONS:
        return "Unavailable For Legal Reasons";
    case StatusCode::INTERNAL_SERVER_ERROR: return "Internal Server Error";
    case StatusCode::NOT_IMPLEMENTED: return "Not Implemented";
    case StatusCode::BAD_GATEWAY: return "Bad Gateway";
    case StatusCode::SERVICE_UNAVAILABLE: return "Service Unavailable";
    case StatusCode::GATEWAY_TIMEOUT: return "Gateway Timeout";
    case StatusCode::HTTP_VERSION_NOT_SUPPORTED:
        return "HTTP Version Not Supported";
    case StatusCode::VARIANT_ALSO_NEGOTIATES: return "Variant Also Negotiates";
    case StatusCode::INSUFFICIENT_STORAGE: return "Insufficient Storage";
    case StatusCode::LOOP_DETECTED: return "Loop Detected";
    case StatusCode::NOT_EXTENDED: return "Not Extended";
    case StatusCode::NETWORK_AUTHENTICATION_REQUIRED:
        return "Network Authentication Required";
    default: return "";
    }
}

} // namespace co_http_uring
