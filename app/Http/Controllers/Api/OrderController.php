<?php

namespace App\Http\Controllers\Api;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Order;
use App\Models\OrderProduct;
use App\Models\OrderTransaction;
use App\Models\OrderProductStock;
use App\Models\OrderProductStockStatus;
use App\Models\OrderProductStockTransaction;
use App\Models\Transaction;
use App\Models\Stock;
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
    public function index()
    {
        return new JsonResource(Order::orderBy('updated_at', 'DESC')->paginate());
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
                        throw new ApiErrorException('Stock not avalable');
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

            $order->note = $request->note;
            $order->status = $request->status;
            $order->customer_id = $request->customer_id;
            $order->save();

            foreach ($request['order_products'] as $_order_product) {
                $order_product_id = $_order_product['id'] ?? null;
                $order_product = $order_product_id ? OrderProduct::find($order_product_id) : new OrderProduct;
                if (!$order_product_id) {
                    $order_product->order_id = $order->id;
                }
                $order_product->product_id = $_order_product['product_id'];
                $order_product->payment_method = $_order_product['payment_method'];
                $order_product->quantity = $order_product_id ? $order_product->quantity
                    : ($_order_product['quantity'] ?? count($_order_product['order_product_stocks']));
                $order_product->save();

                foreach ($_order_product['order_product_stocks'] as $_order_product_stock) {
                    $order_product_stock_id = $_order_product_stock['id'] ?? null;

                    $order_product_stock = $order_product_stock_id ? OrderProductStock::find($order_product_stock_id) : null;

                    $stock_id = $_order_product_stock['stock_id'] ?? null;

                    if (!$order_product_stock || ($order_product_stock->stock_id !== $stock_id)) {
                        $stock = Stock::find($stock_id);

                        if ($stock->quantity <= 0) {
                            throw new ApiErrorException('Stock not available');
                        }

                        $stock->quantity -= 1;
                        $stock->save();
                    }

                    $order_product_stock = $order_product_stock ? $order_product_stock : new OrderProductStock;
                    $order_product_stock->order_product_id = $order_product->id;
                    $order_product_stock->stock_id = $stock_id;
                    $order_product_stock->amount = $_order_product_stock['amount'];
                    $order_product_stock->save();

                    // Transactions
                    foreach ($_order_product_stock['transactions'] as $_transaction) {
                        $transaction_id = $_transaction['id'] ?? null;
                        $transaction = $transaction_id ? Transaction::find($transaction_id) : new Transaction;
                        $transaction->amount = $_transaction['amount'];
                        $transaction->description = $_transaction['description'];
                        $transaction->paid_date = $_transaction['paid_date'];
                        $transaction->cashier_id = $request->user()->id;
                        $transaction->save();

                        if (!$transaction_id) {
                            $order_product_stock_transaction = new OrderProductStockTransaction;
                            $order_product_stock_transaction->order_product_stock_id = $order_product_stock->id;
                            $order_product_stock_transaction->transaction_id = $transaction->id;
                            $order_product_stock_transaction->save();
                        }
                    }
                }
            }

            $_transactions = $request['transactions'];

            foreach ($_transactions as $_transaction) {
                $transaction_id = $_transaction['id'] ?? null;
                $transaction = $transaction_id ? Transaction::find($transaction_id) : new Transaction;
                // $transaction->amount = $_transaction['amount'];
                $transaction->description = $_transaction['description'];
                // $transaction->paid_date = $_transaction['paid_date'];
                $transaction->cashier_id = $request->user()->id;
                $transaction->save();

                if (!$transaction_id) {
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
