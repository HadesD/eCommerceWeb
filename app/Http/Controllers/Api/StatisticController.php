<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\User;
use App\Models\Stock;
use App\Models\Product;
use App\Models\Order;
use App\Models\Transaction;
use App\Models\OrderProductStock;
use App\Models\OrderProductStockTransaction;
use Illuminate\Support\Facades\DB;

class StatisticController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index()
    {
        return [
            'order' => [
                'count' => Order::count(),
                'uncompleted_count' => Order::whereNotIn('status', [Order::STS_CANCELED, Order::STS_COMPLETED])->count(),
            ],
            'user' => [
                'count' => User::count(),
                'admin_count' => User::where('role', '>=', User::ROLE_ADMIN_MANAGER)->count(),
            ],
            'product' => [
                'count' => Product::count(),
                'selling_count' => Product::where('status', Product::STS_SELLING)->count(),
            ],
            'stock' => [
                'count' => Stock::count(),
                'avail_count' => Stock::where('quantity', '>', 0)->count(),
                // 'avail_count_quantity' => DB::select('SELECT SUM(quantity) AS total FROM stocks WHERE quantity > 0')[0]->total,
                // 'avail_cost_price' => DB::select('SELECT SUM(cost_price * quantity) AS total FROM stocks WHERE deleted_at IS NULL')[0]->total,
                'avail_cost_price' => Stock::sum(DB::raw('cost_price * quantity')),
                // 'this_month_cost_price_total' => Stock::where('in_date', '>=', date('Y-m-01'))->sum('cost_price'),
            ],
            'transaction' => [
                'amount_total' => Transaction::sum('amount'),
                'this_month_amount_total' => Transaction::where('paid_date', '>=', date('Y-m-01'))->sum('amount'),
                // 'chart' => Transaction::selectRaw('(YEAR(paid_date)*100+MONTH(paid_date)) AS ym, SUM(amount) AS amount')->orderBy('ym', 'ASC')->groupBy('ym')->query(),
                'chart' => DB::select('SELECT (YEAR(paid_date)*100+MONTH(paid_date)) AS ym, SUM(amount) AS amount FROM transactions WHERE deleted_at IS NOT NULL GROUP BY ym ORDER BY ym ASC'),

                // = sell_price - cost_price
                /*
                'this_month_earning_total' => (function(){
                    $sold_stock_elo = OrderProductStock::select('id', 'stock_id')->whereIn('id', function($q_ops){
                        $q_ops->select('order_product_stock_id')
                            ->from((new OrderProductStockTransaction)->getTable())
                            ->whereIn('transaction_id', function($q_t){
                                $q_t->select('id')
                                    ->from((new Transaction)->getTable())
                                    ->where('paid_date', '>=', date('Y-m-01'));
                            });
                    })->whereIn('id', function($q_ops){
                        $q_ops->select('id')
                            ->from((new Order)->getTable())
                            ->whereIn('status', [Order::STS_PAID, Order::STS_COMPLETED]);
                    });
                    $sold_price = Transaction::whereIn('id', function($q_t) use ($sold_stock_elo){
                        $q_t->select('transaction_id')
                            ->from((new OrderProductStockTransaction)->getTable())
                            ->whereIn('order_product_stock_id', (clone $sold_stock_elo)->pluck('id')->toArray());
                    })//->where('paid_date', '>=', date('Y-m-01'))
                        ->sum('amount');

                    $cost_price = Stock::whereIn('id', (clone $sold_stock_elo)->pluck('stock_id')->toArray())->sum('cost_price');

                    return ($sold_price - $cost_price);
                })(),
                */
            ],
        ];
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        //
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show($id)
    {
        //
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
        //
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
