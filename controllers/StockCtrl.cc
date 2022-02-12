#include "StockCtrl.h"
//add definition of your processing function here

#include "models/Stocks.h"
#include "models/Categories.h"
#include "models/StockCategories.h"

#include "app_helpers/StocksMetaData.hpp"

using Stock = drogon_model::web_rinphone::Stocks;
using Category = drogon_model::web_rinphone::Categories;
using StockCategory = drogon_model::web_rinphone::StockCategories;
using Image = drogon_model::web_rinphone::Images;
using StockImage = drogon_model::web_rinphone::StockImages;

void StockCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

}

void StockCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}

void StockCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{

}

void StockCtrl::updateOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{

}
