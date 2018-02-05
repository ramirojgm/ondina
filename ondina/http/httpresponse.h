/*
	Copyright (C) 2018 Ramiro Jose Garcia Moraga

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HTTP_RESPONSE_
#define HTTP_RESPONSE_

#define HTTP_TYPE_RESPONSE	(http_response_get_type())

G_DECLARE_DERIVABLE_TYPE(HttpResponse,http_response,HTTP,RESPONSE,HttpPackage)

typedef enum _HttpResponseCode HttpResponseCode;

enum _HttpResponseCode
{
  HTTP_RESPONSE_CONTINUE = 100,
  HTTP_RESPONSE_SWITCHING_PROTOCOLS = 101,
  HTTP_RESPONSE_PROCESSING = 102,
  HTTP_RESPONSE_OK = 200,
  HTTP_RESPONSE_CREATED = 201,
  HTTP_RESPONSE_ACCEPTED = 202,
  HTTP_RESPONSE_NON_AUTHORITATIVE_INFORMATION = 203,
  HTTP_RESPONSE_NO_CONTENT = 204,
  HTTP_RESPONSE_RESET_CONTENT = 205,
  HTTP_RESPONSE_PARTIAL_CONTENT = 206,
  HTTP_RESPONSE_MULTI_STATUS = 207,
  HTTP_RESPONSE_ALREADY_REPORTED = 208,
  HTTP_RESPONSE_IM_USED = 226,
  HTTP_RESPONSE_MULTIPLE_CHOICES = 300,
  HTTP_RESPONSE_MOVED_PERMANENTLY = 301,
  HTTP_RESPONSE_FOUND = 302,
  HTTP_RESPONSE_SEE_OTHER = 303,
  HTTP_RESPONSE_NOT_MODIFIED = 304,
  HTTP_RESPONSE_USE_PROXY = 305,
  HTTP_RESPONSE_SWITCH_PROXY = 306,
  HTTP_RESPONSE_TEMPORARY_REDIRECT = 307,
  HTTP_RESPONSE_PERMANENT_REDIRECT = 308,
  HTTP_RESPONSE_BAD_REQUEST = 400,
  HTTP_RESPONSE_UNAUTHORIZED = 401,
  HTTP_RESPONSE_PAYMENT_REQUIRED = 402,
  HTTP_RESPONSE_FORBIDDEN = 403,
  HTTP_RESPONSE_NOT_FOUND = 404,
  HTTP_RESPONSE_METHOD_NOT_ALLOWED = 405,
  HTTP_RESPONSE_NOT_ACCEPTABLE = 406,
  HTTP_RESPONSE_PROXY_AUTHENTICATION_REQUIRED = 407,
  HTTP_RESPONSE_REQUEST_TIME_OUT = 408,
  HTTP_RESPONSE_CONFLICT = 409,
  HTTP_RESPONSE_GONE = 410,
  HTTP_RESPONSE_LENGTH_REQUIRED = 411,
  HTTP_RESPONSE_PRECONDITION_FAILED = 412,
  HTTP_RESPONSE_PLAYLOAD_TOO_LARGE = 413,
  HTTP_RESPONSE_URI_TOO_LONG = 414,
  HTTP_RESPONSE_UNSUPORTED_MEDIA_TYPE = 415,
  HTTP_RESPONSE_RANGE_NOT_SATISFIABLE = 416,
  HTTP_RESPONSE_EXPECTATION_FAILED = 417,
  HTTP_RESPONSE_IM_A_TEAPOT = 418,
  HTTP_RESPONSE_MISDIRECTED_REQUEST = 421,
  HTTP_RESPONSE_UNPROCESSABLE_ENTITY = 422,
  HTTP_RESPONSE_LOCKED = 423,
  HTTP_RESPONSE_FAILED_DEPENDENCY = 424,
  HTTP_RESPONSE_UPGRADE_REQUIRED = 426,
  HTTP_RESPONSE_PRECONDITION_REQUIRED = 428,
  HTTP_RESPONSE_TOO_MANY_REQUESTS = 429,
  HTTP_RESPONSE_REQUEST_HEADER_FIELDS_TO_LARGE = 431,
  HTTP_RESPONSE_UNAVAILABLE_FOR_LEGAL_REASON = 451,
  HTTP_RESPONSE_INTERNAL_SERVER_ERROR = 500,
  HTTP_RESPONSE_NOT_IMPLEMENTED = 501,
  HTTP_RESPONSE_BAD_GATEWAY = 502,
  HTTP_RESPONSE_SERVICE_UNAVAILABLE = 503,
  HTTP_RESPONSE_GATEWAY_TIME_OUT = 504,
  HTTP_RESPONSE_HTTP_VERSION_NOT_SUPPORTED = 505,
  HTTP_RESPONSE_VARIANT_ALSO_NEGOTIATIES = 506,
  HTTP_RESPONSE_INSUFFICIENT_STORAGE = 507,
  HTTP_RESPONSE_LOOP_DETECTED = 508,
  HTTP_RESPONSE_NOT_EXTENDED = 510,
  HTTP_RESPONSE_NETWORK_AUTHENTICATION_REQUIRED = 511,
  HTTP_RESPONSE_INVALID
};

struct _HttpResponseClass
{
	/* parent class */
	HttpPackageClass parent_class;
	/* methods */
	gpointer padding[12];
};

G_BEGIN_DECLS

HttpResponse *		http_response_new(HttpResponseCode code,gdouble version);

HttpResponseCode	http_response_get_code(HttpResponse * response);
void			http_response_set_code(HttpResponse * response,HttpResponseCode code);

gdouble			http_response_get_version(HttpResponse * response);
void			http_response_set_version(HttpResponse * response,gdouble version);

G_END_DECLS

#endif