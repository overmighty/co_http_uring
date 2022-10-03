// Copyright (C) 2022 OverMighty
// SPDX-License-Identifier: Apache-2.0

#include "co_http_uring/status_code.hpp"

namespace co_http_uring {

std::string_view status_code_message(StatusCode code) {
    switch (code) {
        using enum StatusCode;
    case CONTINUE: return "Continue";
    case SWITCHING_PROTOCOLS: return "Switching Protocols";
    case PROCESSING: return "Processing";
    case EARLY_HINTS: return "Early Hints";
    case OK: return "OK";
    case CREATED: return "Created";
    case ACCEPTED: return "Accepted";
    case NON_AUTHORITATIVE_INFORMATION: return "Non-Authoritative Information";
    case NO_CONTENT: return "No Content";
    case RESET_CONTENT: return "Reset Content";
    case PARTIAL_CONTENT: return "Partial Content";
    case MULTI_STATUS: return "Multi-Status";
    case ALREADY_REPORTED: return "Already Reported";
    case IM_USED: return "IM Used";
    case MULTIPLE_CHOICES: return "Multiple Choices";
    case MOVED_PERMANENTLY: return "Moved Permanently";
    case FOUND: return "Found";
    case SEE_OTHER: return "See Other";
    case NOT_MODIFIED: return "Not Modified";
    case USE_PROXY: return "Use Proxy";
    case TEMPORARY_REDIRECT: return "Temporary Redirect";
    case PERMANENT_REDIRECT: return "Permanent Redirect";
    case BAD_REQUEST: return "Bad Request";
    case UNAUTHORIZED: return "Unauthorized";
    case PAYMENT_REQUIRED: return "Payment Required";
    case FORBIDDEN: return "Forbidden";
    case NOT_FOUND: return "Not Found";
    case METHOD_NOT_ALLOWED: return "Method Not Allowed";
    case NOT_ACCEPTABLE: return "Not Acceptable";
    case PROXY_AUTHENTICATION_REQUIRED: return "Proxy Authentication Required";
    case REQUEST_TIMEOUT: return "Request Timeout";
    case CONFLICT: return "Conflict";
    case GONE: return "Gone";
    case LENGTH_REQUIRED: return "Length Required";
    case PRECONDITION_FAILED: return "Precondition Failed";
    case CONTENT_TOO_LARGE: return "Content Too Large";
    case URI_TOO_LONG: return "URI Too Long";
    case UNSUPPORTED_MEDIA_TYPE: return "Unsupported Media Type";
    case RANGE_NOT_SATISFIABLE: return "Range Not Satisfiable";
    case EXPECTATION_FAILED: return "Expectation Failed";
    case MISDIRECTED_REQUEST: return "Misdirected Request";
    case UNPROCESSABLE_CONTENT: return "Unprocessable Content";
    case LOCKED: return "Locked";
    case FAILED_DEPENDENCY: return "Failed Dependency";
    case TOO_EARLY: return "Too Early";
    case UPGRADE_REQUIRED: return "Upgrade Required";
    case PRECONDITION_REQUIRED: return "Precondition Required";
    case TOO_MANY_REQUESTS: return "Too Many Requests";
    case REQUEST_HEADER_FIELDS_TOO_LARGE:
        return "Request Header Fields Too Large";
    case UNAVAILABLE_FOR_LEGAL_REASONS: return "Unavailable For Legal Reasons";
    case INTERNAL_SERVER_ERROR: return "Internal Server Error";
    case NOT_IMPLEMENTED: return "Not Implemented";
    case BAD_GATEWAY: return "Bad Gateway";
    case SERVICE_UNAVAILABLE: return "Service Unavailable";
    case GATEWAY_TIMEOUT: return "Gateway Timeout";
    case HTTP_VERSION_NOT_SUPPORTED: return "HTTP Version Not Supported";
    case VARIANT_ALSO_NEGOTIATES: return "Variant Also Negotiates";
    case INSUFFICIENT_STORAGE: return "Insufficient Storage";
    case LOOP_DETECTED: return "Loop Detected";
    case NOT_EXTENDED: return "Not Extended";
    case NETWORK_AUTHENTICATION_REQUIRED:
        return "Network Authentication Required";
    default: return "";
    }
}

} // namespace co_http_uring
