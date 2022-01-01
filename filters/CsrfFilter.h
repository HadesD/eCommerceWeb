/**
 *
 *  CsrfFilter.h
 *
 */

#pragma once

#include <drogon/HttpFilter.h>
using namespace drogon;


class CsrfFilter : public HttpFilter<CsrfFilter>
{
  public:
    CsrfFilter() {}
    virtual void doFilter(const HttpRequestPtr &req,
                          FilterCallback &&fcb,
                          FilterChainCallback &&fccb) override;
};

