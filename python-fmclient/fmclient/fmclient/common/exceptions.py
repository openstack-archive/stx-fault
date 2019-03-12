#
# Copyright (c) 2018 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#


import inspect
import sys
import six
from six.moves import http_client
from fmclient.common.i18n import _


class ClientException(Exception):
    """An error occurred."""
    def __init__(self, message=None):
        self.message = message

    def __str__(self):
        return self.message or self.__class__.__doc__


class InvalidEndpoint(ClientException):
    """The provided endpoint is invalid."""


class EndpointException(ClientException):
    """Something is rotten in Service Catalog."""


class CommunicationError(ClientException):
    """Unable to communicate with server."""


class Conflict(ClientException):
    """HTTP 409 - Conflict.

    Indicates that the request could not be processed because of conflict
    in the request, such as an edit conflict.
    """
    http_status = http_client.CONFLICT
    message = _("Conflict")


# _code_map contains all the classes that have http_status attribute.
_code_map = dict(
    (getattr(obj, 'http_status', None), obj)
    for name, obj in vars(sys.modules[__name__]).items()
    if inspect.isclass(obj) and getattr(obj, 'http_status', False)
)


class HttpError(ClientException):
    """The base exception class for all HTTP exceptions."""
    http_status = 0
    message = _("HTTP Error")

    def __init__(self, message=None, details=None,
                 response=None, request_id=None,
                 url=None, method=None, http_status=None):
        self.http_status = http_status or self.http_status
        self.message = message or self.message
        self.details = details
        self.request_id = request_id
        self.response = response
        self.url = url
        self.method = method
        formatted_string = "%s (HTTP %s)" % (self.message, self.http_status)
        if request_id:
            formatted_string += " (Request-ID: %s)" % request_id
        super(HttpError, self).__init__(formatted_string)


class HTTPRedirection(HttpError):
    """HTTP Redirection."""
    message = _("HTTP Redirection")


class HTTPClientError(HttpError):
    """Client-side HTTP error.

    Exception for cases in which the client seems to have erred.
    """
    message = _("HTTP Client Error")


class HTTPNotFound(HTTPClientError):
    """HTTP 404 - Not Found

    Exception for cases in which the server did not find anything
    matching the Request-URI.
    """
    message = _("HTTP Client Error: Not Found")


class HttpServerError(HttpError):
    """Server-side HTTP error.

    Exception for cases in which the server is aware that it has
    erred or is incapable of performing the request.
    """
    message = _("HTTP Server Error")


class ServiceUnavailable(HttpServerError):
    """HTTP 503 - Service Unavailable.

    The server is currently unavailable.
    """
    http_status = http_client.SERVICE_UNAVAILABLE
    message = _("Service Unavailable")


class GatewayTimeout(HttpServerError):
    """HTTP 504 - Gateway Timeout.

    The server was acting as a gateway or proxy and did not receive a timely
    response from the upstream server.
    """
    http_status = http_client.GATEWAY_TIMEOUT
    message = "Gateway Timeout"


class HttpVersionNotSupported(HttpServerError):
    """HTTP 505 - HttpVersion Not Supported.

    The server does not support the HTTP protocol version used in the request.
    """
    http_status = http_client.HTTP_VERSION_NOT_SUPPORTED
    message = "HTTP Version Not Supported"


def from_response(response, method, url=None):
    """Returns an instance of :class:`HttpError` or subclass based on response.

    :param response: instance of `requests.Response` class
    :param method: HTTP method used for request
    :param url: URL used for request
    """

    req_id = response.headers.get("x-openstack-request-id")
    kwargs = {
        "http_status": response.status_code,
        "response": response,
        "method": method,
        "url": url,
        "request_id": req_id,
    }
    if "retry-after" in response.headers:
        kwargs["retry_after"] = response.headers["retry-after"]

    content_type = response.headers.get("Content-Type", "")
    if content_type.startswith("application/json"):
        try:
            body = response.json()
        except ValueError:
            pass
        else:
            if isinstance(body, dict):
                error = body.get(list(body)[0])
                if isinstance(error, dict):
                    kwargs["message"] = (error.get("message") or
                                         error.get("faultstring"))
                    kwargs["details"] = (error.get("details") or
                                         six.text_type(body))
    elif content_type.startswith("text/"):
        kwargs["details"] = getattr(response, 'text', '')

    try:
        cls = _code_map[response.status_code]
    except KeyError:
        if 500 <= response.status_code < 600:
            cls = HttpServerError
        elif 404 == response.status_code:
            cls = HTTPNotFound
        elif 400 <= response.status_code < 500:
            cls = HTTPClientError
        else:
            cls = HttpError
    return cls(**kwargs)
