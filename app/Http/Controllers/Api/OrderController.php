<?php

namespace App\Http\Controllers\Api;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Order;
use App\Models\OrderProduct;
use App\Models\OrderTransaction;
use App\Models\OrderProductStock;
use App\Models\OrderProductStockTransaction;
use App\Models\Transaction;
use App\Models\Stock;
use App\Models\User;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

class OrderController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $orderQuery = new Order;

        if (isset($request->status)) {
            $orderQuery = $orderQuery->whereIn('status', $request->status);
        }

        foreach (['note'] as $value) {
            if (isset($request->{$value})) {
                $orderQuery = $orderQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        $orderQuery = $orderQuery->orderBy('deal_date', 'DESC');

        return new JsonResource($orderQuery->paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        $this->validate($request, [
            'note' => 'required',
            'status' => 'required',
            'customer_id' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $order = new Order;
            $order->note = $request->note;
            $order->status = $request->status;
            $order->customer_id = $request->customer_id;
            $order->save();

            foreach ($request['order_products'] as $_order_product) {
                $order_product = new OrderProduct;
                $order_product->order_id = $order->id;
                $order_product->product_id = $_order_product['product_id'];
                $order_product->payment_method = $_order_product['payment_method'];
                $order_product->quantity = $_order_product['quantity'] ?? count($_order_product['order_product_stocks']);
                $order_product->save();

                foreach ($_order_product['order_product_stocks'] as $_order_product_stock) {
                    $stock_id = $_order_product_stock['stock_id'];
                    $status = $_order_product_stock['status'];

                    $stock = Stock::find($stock_id);

                    if ($stock->quantity <= 0) {
                        throw new ApiErrorException("Hàng trong kho không đủ: #{$stock->id}. {$stock->name}");
                    }

                    if ($status === OrderProductStock::STS_SOLD) {
                        $stock->quantity -= 1;
                    }
                    $stock->save();

                    $order_product_stock = new OrderProductStock;
                    $order_product_stock->order_product_id = $order_product->id;
                    $order_product_stock->stock_id = $stock_id;
                    $order_product_stock->status = $status;
                    $order_product_stock->amount = $_order_product_stock['amount'];
                    $order_product_stock->save();

                    // Transactions
                    foreach ($_order_product_stock['transactions'] as $_transaction) {
                        $transaction = new Transaction;
                        $transaction->amount = $_transaction['amount'];
                        $transaction->description = $_transaction['description'];
                        $transaction->paid_date = $_transaction['paid_date'];
                        $transaction->cashier_id = $request->user()->id;
                        $transaction->save();

                        $order_product_stock_transaction = new OrderProductStockTransaction;
                        $order_product_stock_transaction->order_product_stock_id = $order_product_stock->id;
                        $order_product_stock_transaction->transaction_id = $transaction->id;
                        $order_product_stock_transaction->save();
                    }
                }
            }

            foreach ($request['transactions'] as $_transaction) {
                $transaction = new Transaction;
                $transaction->amount = $_transaction['amount'];
                $transaction->description = $_transaction['description'];
                $transaction->paid_date = $_transaction['paid_date'];
                $transaction->cashier_id = $request->user()->id;
                $transaction->save();

                $order_transaction = new OrderTransaction;
                $order_transaction->order_id = $order->id;
                $order_transaction->transaction_id = $transaction->id;
                $order_transaction->save();
            }

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($order);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Order $order)
    {
        return new JsonResource($order);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, Order $order)
    {
        $this->validate($request, [
            'note' => 'required',
            'status' => 'required',
            'customer_id' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $authUser = $request->user();

            $order->note = $request->note;
            $order->status = $request->status;
            $order->customer_id = $request->customer_id;
            $order->save();

            foreach ($request['order_products'] as $_order_product) {
                $order_product = OrderProduct::find($_order_product['id'] ?? null);
                $op_exists = $order_product ? true : false;
                if (!$op_exists) {
                    $order_product = new OrderProduct;
                    $order_product->order_id = $order->id;
                    $order_product->quantity = $_order_product['quantity'] ?? count($_order_product['order_product_stocks']);
                }
                if (!$op_exists || $authUser->hasPermission(User::ROLE_ADMIN_MASTER)) {
                    $order_product->product_id = $_order_product['product_id'];
                }
                $order_product->payment_method = $_order_product['payment_method'];
                $order_product->save();

                foreach ($_order_product['order_product_stocks'] as $_order_product_stock) {
                    $order_product_stock = OrderProductStock::find($_order_product_stock['id'] ?? null);
                    $ops_exists = $order_product_stock ? true : false;
                    if (!$ops_exists) {
                        $order_product_stock = new OrderProductStock;
                        $order_product_stock->order_product_id = $order_product->id;
                        $order_product_stock->stock_id = $_order_product_stock['stock_id'];
                    }

                    $ops_previous_sts = $order_product_stock->status;

                    $order_product_stock->amount = $_order_product_stock['amount'];
                    $order_product_stock->status = (int)$_order_product_stock['status'];
                    $order_product_stock->save();

                    // Stock Quantity Update
                    $stock = Stock::find($order_product_stock->stock_id);
                    if (!$ops_exists || ($ops_previous_sts !== $order_product_stock->status)) {
                        switch ($order_product_stock->status) {
                            case OrderProductStock::STS_SOLD: {
                                if (!$ops_exists || ($ops_previous_sts !== $order_product_stock->status)) {
                                    if ($stock->quantity <= 0) {
                                        throw new ApiErrorException("Hàng trong kho không đủ: #{$stock->id}. {$stock->name}");
                                    }

                                    $stock->quantity -= 1;
                                }
                            } break;
                            case OrderProductStock::STS_REFUND: {
                                if ($ops_exists) {
                                    $stock->quantity += 1;
                                }
                            } break;
                        }
                    }
                    $stock->save();

                    // Stock Transactions
                    foreach ($_order_product_stock['transactions'] as $_transaction) {
                        $transaction = Transaction::find($_transaction['id'] ?? null);
                        $st_exists = $transaction ? true : false;
                        if (!$st_exists) {
                            $transaction = new Transaction;
                            $transaction->amount = $_transaction['amount'];
                            $transaction->paid_date = $_transaction['paid_date'];
                            $transaction->cashier_id = $request->user()->id;
                        }
                        $transaction->description = $_transaction['description'];
                        $transaction->save();

                        if (!$st_exists) {
                            $order_product_stock_transaction = new OrderProductStockTransaction;
                            $order_product_stock_transaction->order_product_stock_id = $order_product_stock->id;
                            $order_product_stock_transaction->transaction_id = $transaction->id;
                            $order_product_stock_transaction->save();
                        }
                    }
                }
            }

            $addon_transactions = $request['transactions'];

            foreach ($addon_transactions as $addon_transaction) {
                $transaction = Transaction::find($addon_transaction['id'] ?? null);
                $at_exists = $transaction ? true : false;
                if (!$at_exists) {
                    $transaction = new Transaction;
                    $transaction->amount = $addon_transaction['amount'];
                    $transaction->paid_date = $addon_transaction['paid_date'];
                    $transaction->cashier_id = $request->user()->id;
                }
                $transaction->description = $addon_transaction['description'];
                $transaction->save();

                if (!$at_exists) {
                    $order_transaction = new OrderTransaction;
                    $order_transaction->order_id = $order->id;
                    $order_transaction->transaction_id = $transaction->id;
                    $order_transaction->save();
                }
            }

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($order);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy($id)
    {
        //
    }
}
