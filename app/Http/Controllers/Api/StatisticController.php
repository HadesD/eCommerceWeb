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
                'avail_count' => Stock::where('status', Stock::STS_AVAILABLE)->count(),
                'cost_price_total' => Stock::sum('cost_price'),
                'this_month_cost_price_total' => Stock::where('in_date', '>=', date('Y-m-01'))->sum('cost_price'),
            ],
            'transaction' => [
                'amount_total' => Transaction::sum('amount'),
                'this_month_amount_total' => Transaction::where('paid_date', '>=', date('Y-m-01'))->sum('amount'),

                // = sell_price - cost_price
                'this_month_earning_total' => (function(){
                    $sold_stock_elo = OrderProductStock::select('id', 'stock_id')->whereIn('id', function($q_ops){
                        $q_ops->select('order_product_stock_id')
                            ->from((new OrderProductStockTransaction)->getTable())
                            ->whereIn('transaction_id', function($q_t){
                                $q_t->select('id')
                                    ->from((new Transaction)->getTable())
                                    ->where('paid_date', '>=', date('Y-m-01'));
                            });
                    });//->pluck('id')->toArray();
                    $sold_price = Transaction::whereIn('id', function($q_t) use ($sold_stock_elo){
                        $q_t->select('transaction_id')
                            ->from((new OrderProductStockTransaction)->getTable())
                            ->whereIn('order_product_stock_id', (clone $sold_stock_elo)->pluck('id')->toArray());
                    })//->where('paid_date', '>=', date('Y-m-01'))
                        ->sum('amount');

                    $cost_price = Stock::whereIn('id', (clone $sold_stock_elo)->pluck('stock_id')->toArray())->sum('cost_price');

                    return ($sold_price - $cost_price);
                })(),
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
