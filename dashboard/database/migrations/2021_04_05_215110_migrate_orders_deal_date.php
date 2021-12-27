<?php

use App\Models\Order;
use Illuminate\Database\Migrations\Migration;
use Illuminate\Support\Facades\DB;

class MigrateOrdersDealDate extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        DB::beginTransaction();
        $orders = Order::all();
        foreach ($orders as $order) {
            $addon_tnx = $order->transactions[0] ?? null;
            $deal_date = $addon_tnx ? $addon_tnx->paid_date : null;
            if (!$deal_date) {
                foreach ($order->order_products as $op) {
                    foreach ($op->order_product_stocks as $ops) {
                        foreach ($ops->transactions as $ops_tnx) {
                            $deal_date = $ops_tnx->paid_date;
                            break;
                        }
                        if ($deal_date) break;
                    }
                    if ($deal_date) break;
                }
            }
            if ($deal_date) {
                $order->deal_date = $deal_date;
                $order->save();
            }
        }
        DB::commit();
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        //
    }
}
