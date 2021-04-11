<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\User;
use App\Models\Stock;
use App\Models\Product;
use App\Models\Order;
use App\Models\Transaction;
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
                'avail_cost_price' => Stock::sum(DB::raw('cost_price * quantity')),
            ],
            'transaction' => [
                'amount_total' => Transaction::sum('amount'),
                'remaining_need_paid_total' => (function() {
                    $orders = Order::where('status', Order::STS_PAYING)->get()->append(['need_paid_total', 'stock_earned_total']);
                    return (clone $orders)->sum('need_paid_total') - (clone $orders)->sum('stock_earned_total');
                })(),
                'this_month_amount_total' => Transaction::where('paid_date', '>=', date('Y-m-01'))->sum('amount'),
                'chart' => Transaction::select(DB::raw('(YEAR(paid_date)*100+MONTH(paid_date)) AS ym'), DB::raw('SUM(amount) AS amount'))->groupBy('ym')->orderBy('ym', 'ASC')->get(),
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
