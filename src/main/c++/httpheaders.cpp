#include "httpheaders.h"

const QByteArray HttpHeaders::ACCEPT = "accept";
const QByteArray HttpHeaders::CONTENT_TYPE = "content-type";
const QByteArray HttpHeaders::CONTENT_LENGTH = "content-length";
const QByteArray HttpHeaders::CACHE_CONTROL = "cache-control";
const QByteArray HttpHeaders::NO_CACHE = "no-cache";

const int HttpHeaders::STATUS_SUCCESS = 200;
const int HttpHeaders::STATUS_NOT_FOUND = 404;
const int HttpHeaders::STATUS_PRECONDITION_FAILED = 412;
const int HttpHeaders::STATUS_UNSUPPORTED_MEDIA_TYPE = 415;
const int HttpHeaders::STATUS_ERROR = 500;
