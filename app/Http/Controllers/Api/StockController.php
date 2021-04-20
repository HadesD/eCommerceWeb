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
        $stockQuery = isset($request->category_id) ? Category::find($request->category_id)->stocks
                : (new Stock);

        foreach (['name', 'idi', 'id'] as $value) {
            if (isset($request->{$value})) {
                $stockQuery = $stockQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        if (isset($request->tester_id)) {
            $stockQuery = $stockQuery->where($value, $request->tester_id);
        }

        if (isset($request->sort_by)) {
            foreach (explode(',', $request->sort_by) as $sorter) {
                $col = substr($sorter, 1);
                $sortType = ($sorter[0] === '-') ? 'DESC' : 'ASC';
                $stockQuery = $stockQuery->orderBy($col, $sortType);
            }
        } else {
            $stockQuery = $stockQuery->orderBy('created_at', 'DESC');
        }

        if (isset($request->download)) {
            $stockQuery = $stockQuery->get()->append('cost_total');

            switch ($request->download) {
                case 'csv': {
                    $csv = \League\Csv\Writer::createFromFileObject(new \SplTempFileObject);
                    $csv->insertOne(array_keys($stockQuery[0]->toArray()));

                    foreach ($stockQuery as $stock) {
                        $csv->insertOne($stock->toArray());
                    }

                    return response((string)$csv, 200, [
                        'Content-Type' => 'text/csv',
                        'Content-Transfer-Encoding' => 'binary',
                        'Content-Disposition' => 'attachment; filename="'.parse_url(config('app.url'))['host'].'_stocks_'.date('Y-m-d_H-i-s').'.csv"',
                    ]);
                } break;
            }
        }

        $stockQuery = $stockQuery->paginate();

        $stockQuery->append(['categories', 'updated_user', 'tester']);

        return new JsonResource($stockQuery);
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        // $this->middleware('role.manager');

        $this->validate($request, [
            // 'note' => 'required',
            'name' => 'required',
            'sell_price' => 'required',
            'cost_price' => 'required',
            'tester_id' => 'required',
            'quantity' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $authUser = $request->user();

            $stock = new Stock;
            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->cost_price = ($request->cost_price < 0) ? 0 : $request->cost_price;
            $stock->updated_user_id = $authUser->id;
            $stock->note = $request->note;
            $stock->tester_id = $request->tester_id;
            $stock->sell_price = $request->sell_price;
            $stock->save();

            // Category
            foreach ($request->categories_id as $category_id) {
                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }

            // Import Addon transaction
            $transaction = new Transaction;
            $transaction->description = "Kho #{$stock->id}: Nhập {$stock->quantity} cái (VND)";
            $transaction->amount = -($stock->quantity * $stock->cost_price);
            $transaction->paid_date = $request->inout_date;
            $transaction->cashier_id = $authUser->id;
            $transaction->save();
            $stock_transaction = new StockTransaction;
            $stock_transaction->stock_id = $stock->id;
            $stock_transaction->transaction_id = $transaction->id;
            $stock_transaction->save();

            // Addon transactions
            foreach ($request->transactions as $_transaction) {
                $transaction = new Transaction;
                $transaction->description = $_transaction['description'];
                $transaction->amount = $_transaction['amount'];
                $transaction->paid_date = $_transaction['paid_date'];
                $transaction->cashier_id = $authUser->id;
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
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Stock $stock)
    {
        return new JsonResource($stock->append(['transactions', 'categories', 'updated_user', 'orders_history', 'tester']));
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
        // $this->middleware('role.manager');

        $this->validate($request, [
            // 'note' => 'required',
            'name' => 'required',
            'sell_price' => 'required',
            'cost_price' => 'required',
            'tester_id' => 'required',
            'quantity' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $authUser = $request->user();

            $prev_quantity = $stock->quantity;

            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->updated_user_id = $authUser->id;
            $stock->note = $request->note;
            if ($authUser->hasPermission(User::ROLE_ADMIN_MASTER)) {
                $stock->cost_price = $request->cost_price;
            }
            if ($authUser->hasPermission(User::ROLE_ADMIN_MANAGER)) {
                $stock->tester_id = $request->tester_id;
                $stock->sell_price = $request->sell_price;
            }
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
                $transaction->cashier_id = $authUser->id;
                $transaction->save();

                $stock_transaction = new StockTransaction;
                $stock_transaction->stock_id = $stock->id;
                $stock_transaction->transaction_id = $transaction->id;
                $stock_transaction->save();
            }

            // Addon Transactions
            foreach ($request->transactions as $_transaction) {
                $transaction_id = $_transaction['id'] ?? null;
                if ($transaction_id && !$authUser->hasPermission(User::ROLE_ADMIN_SUB_MASTER)) {
                    continue;
                }

                $transaction = Transaction::find($transaction_id);

                $tnx_exists = $transaction ? true : false;
                if (!$tnx_exists) {
                    $transaction = new Transaction;
                }

                $transaction->description = $_transaction['description'];
                $transaction->amount = $_transaction['amount'];
                $transaction->paid_date = $_transaction['paid_date'];
                if (!$tnx_exists) {
                    $transaction->cashier_id = $authUser->id;
                }
                $transaction->save();

                if (!$tnx_exists) {
                    $stock_transaction = new StockTransaction;
                    $stock_transaction->stock_id = $stock->id;
                    $stock_transaction->transaction_id = $transaction->id;
                    $stock_transaction->save();
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
