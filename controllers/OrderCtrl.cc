#include "OrderCtrl.h"

#include "models/Products.h"
#include "models/Orders.h"
#include "models/OrderProducts.h"
#include "models/Users.h"

#include "app_helpers/OrdersMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Order = drogon_model::web_rinphone::Orders;
using OrderProduct = drogon_model::web_rinphone::OrderProducts;
using User = drogon_model::web_rinphone::Users;
using namespace app_helpers;

void OrderCtrl::get(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::getOne(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, uint64_t id)
{
    callback(HttpResponse::newNotFoundResponse());
}

void OrderCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    const auto& reqJsonPtr = req->getJsonObject();
    if (!reqJsonPtr)
    {
        auto ret = HttpResponse::newHttpResponse();
        ret->setStatusCode(HttpStatusCode::k406NotAcceptable);
        callback(ret);
        return;
    }

    const auto& reqJson = *reqJsonPtr;
    const auto& reqItems = reqJson["items"];

    if (!reqItems.isArray())
    {
        auto ret = HttpResponse::newHttpResponse();
        ret->setStatusCode(HttpStatusCode::k406NotAcceptable);
        callback(ret);
        return;
    }

    Json::Value resJson;
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;
    auto& resMsg = resJson["message"];

    {
        auto dbClient = app().getDbClient()->newTransaction();
        try
        {
            const auto payMethod = static_cast<PaymentMethod>(reqJson["payment_method"].asUInt());
            switch (payMethod)
            {
            case PaymentMethod::ONCE:
            case PaymentMethod::INSTALMENT:
                break;

            default:
                throw std::logic_error("Hình thức thanh toán không tồn tại");
            }

            const auto& reqNote = reqJson["note"];

            Order ord;
            ord.setStatus(static_cast<uint8_t>(OrderStatus::PROCESSING));
            ord.setCustomerId(0); // TODO: Set current user id
            if (reqNote.isString())
            {
                const auto& noteStr = reqNote.asString();
                constexpr auto maxNoteSize = 500;
                if (noteStr.size() > maxNoteSize)
                {
                    ord.setNote(noteStr.substr(maxNoteSize));
                }
                else
                {
                    ord.setNote(noteStr);
                }
            }

            const auto now = trantor::Date::now();
            ord.setDealDate(now);
            ord.setCreatedAt(now);
            ord.setUpdatedAt(now);

            orm::Mapper<Order> ordMap(dbClient);

            ordMap.insert(ord);

            LOG_DEBUG << ord.getValueOfId();

            orm::Mapper<Product> prdMap(dbClient);
            orm::Mapper<OrderProduct> ordPrdMap(dbClient);

            for (const auto& item : reqItems)
            {
                const auto& prd = prdMap.findByPrimaryKey(item["product_id"].asUInt64());

                // OrderProduct
                OrderProduct ordPrd;
                ordPrd.setOrderId(ord.getValueOfId());
                ordPrd.setProductId(prd.getValueOfId());
                ordPrd.setQuantity(item["num"].asUInt());
                ordPrd.setPaymentMethod(static_cast<uint8_t>(payMethod));

                ordPrdMap.insert(ordPrd);
            }

            // throw std::logic_error("ERR"); // TODO: remove this
        }
        catch (const orm::UnexpectedRows& e)
        {
            dbClient->rollback();

            httpRetCode = HttpStatusCode::k500InternalServerError;

            resMsg = "Dữ liệu sai hoặc không tìm thấy";
        }
        catch (const std::logic_error& e)
        {
            dbClient->rollback();

            httpRetCode = HttpStatusCode::k500InternalServerError;

            resMsg = e.what();
        }
        catch (const std::exception &e)
        {
            dbClient->rollback();

            LOG_ERROR << e.what();

            httpRetCode = HttpStatusCode::k500InternalServerError;

            resMsg = "Lỗi không xác định";
        }
        catch (...)
        {
            dbClient->rollback();

            httpRetCode = HttpStatusCode::k500InternalServerError;

            resMsg = "Lỗi không xác định";
        }
    }

    const auto& httpRet = HttpResponse::newHttpJsonResponse(resJson);
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
