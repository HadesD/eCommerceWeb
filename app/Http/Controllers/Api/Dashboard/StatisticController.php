<?php

namespace App\Http\Controllers\Api\Dashboard;

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
    const FUNDS = 500000000;

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
                'funds' => self::FUNDS,

                'chart_total' => Transaction::select(DB::raw('(YEAR(paid_date)*100+MONTH(paid_date)) AS ym'), DB::raw('SUM(amount) AS amount'))->groupBy('ym')->orderBy('ym', 'ASC')->get(),
                'chart_near_30_days' => Transaction::select(DB::raw('(YEAR(paid_date)*10000+MONTH(paid_date)*100+DAY(paid_date)) AS ymd'), DB::raw('SUM(amount) AS amount'))->where('paid_date','>=', date('Y-m-d 00:00:00', strtotime('-30 days')))->groupBy('ymd')->orderBy('ymd', 'ASC')->get(),

                'amount_total' => Transaction::sum('amount'),
                'remaining_need_paid_total' => Order::where('status', Order::STS_PAYING)->get()->append('remaining_need_paid_total')->sum('remaining_need_paid_total'),
                'this_month_amount_total' => Transaction::where('paid_date', '>=', date('Y-m-01 00:00:00'))->sum('amount'),

                'real_amount_total_before_debt' => 0,
                'real_amount_total_after_debt' => 0,
            ],
        ];
    }
}
