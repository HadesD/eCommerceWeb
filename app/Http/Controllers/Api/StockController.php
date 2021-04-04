<?php

namespace App\Http\Controllers\Api;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Stock;
use App\Models\Category;
use App\Models\StockCategory;
use App\Models\StockTransaction;
use App\Models\Transaction;
use App\Models\User;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

class StockController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $stockQuery = isset($request->category_id) ? Category::find($request->category_id)->stocks->orderBy('id', 'DESC')
                : Stock::orderBy('id', 'DESC');

        foreach (['name', 'idi'] as $value) {
            if (isset($request->{$value})) {
                $stockQuery = $stockQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        return new JsonResource($stockQuery->paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        try {
            DB::beginTransaction();

            $stock = new Stock;
            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->cost_price = ($request->cost_price < 0) ? 0 : $request->cost_price;
            $stock->updated_user_id = $request->user()->id;
            $stock->note = $request->note;
            $stock->save();

            // Category
            foreach ($request->categories_id as $category_id) {
                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }

            $transaction = new Transaction;
            $transaction->description = "Kho #{$stock->id}: Nhập {$stock->quantity} cái (VND)";
            $transaction->amount = -($stock->quantity * $stock->cost_price);
            $transaction->paid_date = $request->inout_date;
            $transaction->cashier_id = $request->user()->id;
            $transaction->save();

            $stock_transaction = new StockTransaction;
            $stock_transaction->stock_id = $stock->id;
            $stock_transaction->transaction_id = $transaction->id;
            $stock_transaction->save();

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

        return $this->show($stock);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Stock $stock)
    {
        return new JsonResource($stock->append(['transactions']));
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, Stock $stock)
    {
        try {
            DB::beginTransaction();

            $prev_quantity = $stock->quantity;

            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->updated_user_id = $request->user()->id;
            $stock->note = $request->note;
            $stock->save();

            // Category
            foreach ($request->categories_id as $category_id) {
                $stock_category = StockCategory::where('stock_id', $stock->id)
                    ->where('category_id', $category_id)
                    ->first();
                if ($stock_category) {
                    continue;
                }

                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }
            StockCategory::where('stock_id', $stock->id)
                ->whereNotIn('category_id', $request->categories_id)
                ->delete();

            $diff_quantity = $prev_quantity - $stock->quantity;

            // Import / Export Transaction
            if ($diff_quantity !== 0) {
                $transaction = new Transaction;
                $transaction->description = "Kho #{$stock->id}: " .
                    (($diff_quantity < 0) ? 'Nhập '.abs($diff_quantity).' cái (VND)' : 'Trả '.abs($diff_quantity).' cái (VND)');
                $transaction->amount = $diff_quantity * $stock->cost_price;
                $transaction->paid_date = $request->inout_date;
                $transaction->cashier_id = $request->user()->id;
                $transaction->save();

                $stock_transaction = new StockTransaction;
                $stock_transaction->stock_id = $stock->id;
                $stock_transaction->transaction_id = $transaction->id;
                $stock_transaction->save();
            }

            // Addon Transactions
            foreach ($request->transactions as $_transaction) {
                if (isset($_transaction['id'])) {
                    continue;
                }

                $transaction = new Transaction;
                $transaction->description = $_transaction['description'];
                $transaction->amount = $_transaction['amount'];
                $transaction->paid_date = $_transaction['paid_date'];
                $transaction->cashier_id = $request->user()->id;
                $transaction->save();

                $stock_transaction = new StockTransaction;
                $stock_transaction->stock_id = $stock->id;
                $stock_transaction->transaction_id = $transaction->id;
                $stock_transaction->save();
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

        return $this->show($stock);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy(Stock $stock)
    {
        //
    }
}
