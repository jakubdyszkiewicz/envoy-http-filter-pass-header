#include <string>

#include "http_filter.h"

#include "envoy/server/filter_config.h"

namespace Envoy {
namespace Http {

HttpSampleDecoderFilterConfig::HttpSampleDecoderFilterConfig(
    const sample::Decoder& proto_config)
    : headers_(proto_config.headers()) {}

HttpSampleDecoderFilter::HttpSampleDecoderFilter(HttpSampleDecoderFilterConfigSharedPtr config)
    : config_(config) {}

HttpSampleDecoderFilter::~HttpSampleDecoderFilter() {}

void HttpSampleDecoderFilter::onDestroy() {}

const std::string HttpSampleDecoderFilter::headers() const {
  return config_->headers();
}

FilterHeadersStatus HttpSampleDecoderFilter::decodeHeaders(HeaderMap& headers, bool) {
  // add a header
  // headers.addCopy(headerKey(), headerValue());
  HeaderEntry* reqIdHeader = headers.get(Envoy::Http::LowerCaseString("x-request-id"));
  if (reqIdHeader != NULL) {
    absl::string_view requestId = reqIdHeader->value().getStringView();
    HeaderEntry* chiContextHeader = headers.get(Envoy::Http::LowerCaseString(this->headers()));
    if (chiContextHeader != NULL) {
      absl::string_view headerToForward = chiContextHeader->value().getStringView();
      numByRequestID[requestId] = headerToForward;
    } else {
      std::string headerToForward = std::string(numByRequestID[requestId]);
      headers.addCopy(Envoy::Http::LowerCaseString("chi-context"), headerToForward);
    }
  }
  return FilterHeadersStatus::Continue;
}

FilterDataStatus HttpSampleDecoderFilter::decodeData(Buffer::Instance&, bool) {
  return FilterDataStatus::Continue;
}

FilterTrailersStatus HttpSampleDecoderFilter::decodeTrailers(HeaderMap&) {
  return FilterTrailersStatus::Continue;
}

void HttpSampleDecoderFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) {
  decoder_callbacks_ = &callbacks;
}

FilterHeadersStatus HttpSampleDecoderFilter::encodeHeaders(HeaderMap& headers, bool) {
    // add a header
    // headers.addCopy(headerKey(), headerValue());
    HeaderEntry* reqIdHeader = headers.get(Envoy::Http::LowerCaseString("x-request-id"));
    if (reqIdHeader != NULL) {
      absl::string_view requestId = reqIdHeader->value().getStringView();
      HeaderEntry* chiContextHeader = headers.get(Envoy::Http::LowerCaseString(this->headers()));
      if (chiContextHeader != NULL) {
        absl::string_view headerToForward = chiContextHeader->value().getStringView();
        numByRequestID[requestId] = headerToForward;
      } else {
        std::string headerToForward = std::string(numByRequestID[requestId]);
        headers.addCopy(Envoy::Http::LowerCaseString("chi-context"), headerToForward);
      }
    }
    return FilterHeadersStatus::Continue;
}

FilterHeadersStatus HttpSampleDecoderFilter::encode100ContinueHeaders(HeaderMap&) {
    return FilterHeadersStatus::Continue;
}

FilterDataStatus HttpSampleDecoderFilter::encodeData(Buffer::Instance&, bool) {
    return FilterDataStatus::Continue;
}

FilterTrailersStatus HttpSampleDecoderFilter::encodeTrailers(HeaderMap&) {
    return FilterTrailersStatus::Continue;
}

FilterMetadataStatus HttpSampleDecoderFilter::encodeMetadata(MetadataMap&) {
    return FilterMetadataStatus::Continue;
}

void HttpSampleDecoderFilter::setEncoderFilterCallbacks(StreamEncoderFilterCallbacks&) {
    return;
}

} // namespace Http
} // namespace Envoy
