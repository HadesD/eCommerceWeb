<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Http\Resources\Order as OrderResource;
use App\Models\Order;
use App\Models\OrderProduct;
use App\Models\OrderTransaction;
use App\Models\OrderProductStock;
use App\Models\OrderProductStockTransaction;
use App\Models\Transaction;
use App\Models\Stock;

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
        return new OrderResource(Order::orderBy('updated_at', 'DESC')->paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        try
        {
            \DB::beginTransaction();

            $order = new Order;
            $order->note = $request->note;
            $order->status = $request->status;
            $order->customer_id = $request->customer_id;
            $order->save();

            foreach ($request['order_products'] as $_order_product)
            {
                $order_product = new OrderProduct;
                $order_product->order_id = $order->id;
                $order_product->product_id = $_order_product['product_id'];
                $order_product->payment_method = $_order_product['payment_method'];
                $order_product->quantity = $_order_product['quantity'] ?? count($_order_product['order_product_stocks']);
                $order_product->save();

                foreach ($_order_product['order_product_stocks'] as $_order_product_stock)
                {
                    $stock_id = $_order_product_stock['stock_id'];

                    $stock = Stock::find($stock_id);

                    if ($stock->status !== Stock::STS_AVAILABLE)
                    {
                        throw new \RuntimeException('Stock not avalable');
                    }

                    $stock->status = Stock::STS_SOLD;
                    $stock->save();

                    $order_product_stock = new OrderProductStock;
                    $order_product_stock->order_product_id = $order_product->id;
                    $order_product_stock->stock_id = $stock_id;;
                    $order_product_stock->amount = $_order_product_stock['amount'];
                    $order_product_stock->save();

                    // Transactions
                    foreach ($_order_product_stock['transactions'] as $_transaction)
                    {
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

            foreach ($request['transactions'] as $_transaction)
            {
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

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            throw new \RuntimeException($e);
        }

        return new OrderResource($order ?? []);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show($id)
    {
        return new OrderResource(Order::find($id));
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, $id)
    {
        try
        {
            \DB::beginTransaction();

            $order = Order::find($id);
            $order->note = $request->note;
            $order->status = $request->status;
            $order->customer_id = $request->customer_id;
            $order->save();

            $order_product_ids_keep = [];
            $order_product_stock_ids_keep = [];
            $order_product_stock_transaction_ids_keep = [];

            foreach ($request['order_products'] as $_order_product)
            {
                $order_product_id = $_order_product['id'] ?? null;
                $order_product = $order_product_id ? OrderProduct::find($order_product_id) : new OrderProduct;
                if (!$order_product_id)
                {
                    $order_product->order_id = $order->id;
                }
                $order_product->product_id = $_order_product['product_id'];
                $order_product->payment_method = $_order_product['payment_method'];
                $order_product->quantity = $order_product_id ? $order_product->quantity : ($_order_product['quantity'] ?? count($_order_product['order_product_stocks']));
                $order_product->save();

                // Push to keep
                $order_product_ids_keep[] = $order_product->id;

                foreach ($_order_product['order_product_stocks'] as $_order_product_stock)
                {
                    $order_product_stock_id = $_order_product_stock['id'] ?? null;

                    $order_product_stock = $order_product_stock_id ? OrderProductStock::find($order_product_stock_id) : null;

                    $stock_id = $_order_product_stock['stock_id'] ?? null;

                    if (!$order_product_stock || ($order_product_stock->stock_id !== $stock_id))
                    {
                        $stock = Stock::find($stock_id);

                        if ($stock->status !== Stock::STS_AVAILABLE)
                        {
                            throw new \RuntimeException('Stock not avalable');
                        }
                        $stock->status = Stock::STS_SOLD;
                        $stock->save();
                    }

                    $order_product_stock = $order_product_stock ? $order_product_stock : new OrderProductStock;
                    $order_product_stock->order_product_id = $order_product->id;
                    $order_product_stock->stock_id = $stock_id;;
                    $order_product_stock->amount = $_order_product_stock['amount'];
                    $order_product_stock->save();

                    // Push to keep
                    $order_product_stock_ids_keep[] = $order_product_stock->id;

                    // Transactions
                    foreach ($_order_product_stock['transactions'] as $_transaction)
                    {
                        $transaction_id = $_transaction['id'] ?? null;
                        $transaction = $transaction_id ? Transaction::find($transaction_id) : new Transaction;
                        $transaction->amount = $_transaction['amount'];
                        $transaction->description = $_transaction['description'];
                        $transaction->paid_date = $_transaction['paid_date'];
                        $transaction->cashier_id = $request->user()->id;
                        $transaction->save();

                        $order_product_stock_transaction_ids_keep[] = $transaction->id;

                        if (!$transaction_id)
                        {
                            $order_product_stock_transaction = new OrderProductStockTransaction;
                            $order_product_stock_transaction->order_product_stock_id = $order_product_stock->id;
                            $order_product_stock_transaction->transaction_id = $transaction->id;
                            $order_product_stock_transaction->save();
                        }
                    }
                }
            }

            $transaction_ids_keep = [];

            $_transactions = $request['transactions'];

            foreach ($_transactions as $_transaction)
            {
                $transaction_id = $_transaction['id'] ?? null;
                $transaction = $transaction_id ? Transaction::find($transaction_id) : new Transaction;
                $transaction->amount = $_transaction['amount'];
                $transaction->description = $_transaction['description'];
                $transaction->paid_date = $_transaction['paid_date'];
                $transaction->cashier_id = $request->user()->id;
                $transaction->save();

                $transaction_ids_keep[] = $transaction->id;

                if (!$transaction_id)
                {
                    $order_transaction = new OrderTransaction;
                    $order_transaction->order_id = $order->id;
                    $order_transaction->transaction_id = $transaction->id;
                    $order_transaction->save();
                }
            }

            // Delete order_product
            foreach ($order->order_products as $_order_product)
            {
                // Delete order_product_stock
                foreach ($_order_product->order_product_stocks as $_order_product_stock)
                {
                    // Delete transactions
                    foreach ($_order_product_stock->transactions as $_transaction)
                    {
                        if (!in_array($_transaction->id, $order_product_stock_transaction_ids_keep))
                        {
                            OrderProductStockTransaction::where('order_product_stock_id', $_order_product_stock->id)
                                ->where('transaction_id', $_transaction->id)
                                ->delete();
                            $_transaction->delete();
                        }
                    }

                    // Delete order_product_stocks
                    if (!in_array($_order_product_stock->id, $order_product_stock_ids_keep))
                    {
                        // Reset item
                        $stock = Stock::find($_order_product_stock->stock_id);
                        $stock->status = Stock::STS_AVAILABLE;
                        $stock->save();

                        $_order_product_stock->delete();
                    }
                }

                if (!in_array($_order_product->id, $order_product_ids_keep))
                {
                    $_order_product->delete();
                }
            }

            // Delete transactions
            foreach ($order->transactions as $_transaction)
            {

                if (!in_array($_transaction->id, $transaction_ids_keep))
                {
                    OrderTransaction::where('order_id', $order->id)
                        ->where('transaction_id', $_transaction->id)
                        ->delete();
                    $_transaction->delete();
                }
            }

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            Log::error($e);

            throw new \RuntimeException($e);
        }

        return new OrderResource($order ?? []);
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
