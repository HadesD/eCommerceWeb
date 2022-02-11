#include "CartCtrl.h"
//add definition of your processing function here

#include "models/Products.h"
#include "models/Orders.h"
#include "models/OrderProducts.h"
#include "models/Users.h"

#include "app_helpers/OrdersMetaData.hpp"
#include "app_helpers/UsersMetaData.hpp"

using Product = drogon_model::web_rinphone::Products;
using Order = drogon_model::web_rinphone::Orders;
using OrderProduct = drogon_model::web_rinphone::OrderProducts;
using User = drogon_model::web_rinphone::Users;
using namespace app_helpers;

void CartCtrl::create(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    Json::Value resJson;
    auto& resMsg = resJson["message"];
    HttpStatusCode httpRetCode = HttpStatusCode::k200OK;

    try
    {
        // Validate
        const auto &reqJsonPtr = req->getJsonObject();
        if (!reqJsonPtr)
        {
            throw std::logic_error("Dữ liệu gửi lên không đúng");
        }

        const auto &reqJson = *reqJsonPtr;
        const auto &reqItems = reqJson["items"];

        if (!reqItems.isArray())
        {
            throw std::logic_error("Không tìm thấy dữ liệu giỏ hàng");
        }

        const auto &paymentInfo = reqJson["payment"];
        if (!paymentInfo.isObject())
        {
            throw std::logic_error("Không tìm thấy thông tin thanh toán");
        }

        const auto &paymentMethod = paymentInfo["method"];
        if (!paymentMethod.isNumeric())
        {
            throw std::logic_error("Không tìm thấy phương thức thanh toán");
        }

        const auto payMethod = static_cast<PaymentMethod>(paymentMethod.asUInt());
        switch (payMethod)
        {
        case PaymentMethod::ONCE:
        case PaymentMethod::INSTALMENT:
        break;

        default:
        {
            throw std::logic_error("Không tìm thấy phương thức thanh toán");
        }
        }

        const auto& phone = paymentInfo["phone"];
        if (!phone.isString())
        {
            throw std::logic_error("Không tìm thấy số điện thoại liên lạc");
        }

        // Completed valid
        {
            auto dbClient = app().getDbClient()->newTransaction();
            try
            {
                const auto now = trantor::Date::now();

                orm::Mapper<User> userMap(dbClient);
                User paymentUser;
                const auto& puName = paymentInfo["fullname"];
                if (puName.isString())
                {
                    paymentUser.setName(puName.asString());
                }
                else
                {
                    paymentUser.setName(utils::genRandomString(1));
                }
                paymentUser.setPhone(phone.asString());
                paymentUser.setPassword(utils::genRandomString(62));
                paymentUser.setRole(static_cast<uint8_t>(UserRole::USER_NORMAL));

                // SNS Info
                {
                    Json::StreamWriterBuilder builder;
                    builder["commentStyle"] = "None";
                    builder["indentation"] = "";
                    Json::Value snsInfo(Json::objectValue);

                    const auto &fb = paymentInfo["facebook"];
                    if (fb.isString())
                    {
                        snsInfo["facebook"] = fb.asString();
                    }

                    paymentUser.setSnsInfo(Json::writeString(builder, snsInfo));
                }

                auto emailRandStr = utils::genRandomString(5);
                const auto& puEmail = paymentInfo["email"];
                if (puEmail.isString())
                {
                    emailRandStr = puEmail.asString() + "@" + emailRandStr;
                }
                paymentUser.setEmail(emailRandStr);
                paymentUser.setCreatedAt(now);
                paymentUser.setUpdatedAt(now);
                userMap.insert(paymentUser);

                const auto &reqNote = reqJson["note"];

                Order ord;
                ord.setStatus(static_cast<uint8_t>(OrderStatus::PROCESSING));
                // TODO: Logged in user
                ord.setCustomerId(paymentUser.getValueOfId());
                if (reqNote.isString())
                {
                    const auto &noteStr = reqNote.asString();
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

                ord.setDealDate(now);
                ord.setCreatedAt(now);
                ord.setUpdatedAt(now);

                orm::Mapper<Order> ordMap(dbClient);

                ordMap.insert(ord);

                orm::Mapper<Product> prdMap(dbClient);
                orm::Mapper<OrderProduct> ordPrdMap(dbClient);

                for (const auto &item : reqItems)
                {
                    const auto &prd = prdMap.findByPrimaryKey(item["product_id"].asUInt64());

                    // OrderProduct
                    OrderProduct ordPrd;
                    ordPrd.setOrderId(ord.getValueOfId());
                    ordPrd.setProductId(prd.getValueOfId());
                    ordPrd.setQuantity(item["num"].asUInt());
                    ordPrd.setPaymentMethod(static_cast<uint8_t>(payMethod));

                    ordPrdMap.insert(ordPrd);
                }

                auto &resData = resJson["data"];
                resData = ord.toJson();
            }
            catch (const orm::UnexpectedRows &e)
            {
                dbClient->rollback();

                httpRetCode = HttpStatusCode::k500InternalServerError;

                resMsg = "Dữ liệu sai hoặc không tìm thấy";
            }
            catch (const std::logic_error &e)
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
    }
    catch (const std::logic_error &e)
    {
        resMsg = e.what();
        httpRetCode = HttpStatusCode::k406NotAcceptable;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();

        resMsg = "Lỗi hệ thống";

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }
    catch (...)
    {
        resMsg = "Liên tiếp thất bại";

        httpRetCode = HttpStatusCode::k500InternalServerError;
    }

    const auto &httpRet = HttpResponse::newHttpJsonResponse(resJson);
    httpRet->setStatusCode(httpRetCode);

    callback(httpRet);
}
