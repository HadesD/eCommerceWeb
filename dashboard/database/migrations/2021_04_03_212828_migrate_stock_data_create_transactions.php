<?php

use App\Models\StockTransaction;
use App\Models\Transaction;
use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class MigrateStockDataCreateTransactions extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::table('stocks', function (Blueprint $table) {
            $allStocks = \App\Models\Stock::all();
            foreach ($allStocks as $stock) {
                $quantity = $stock->quantity;

                foreach ($stock->products as $product) {
                    $quantity += 1;
                }

                $transaction = new Transaction;
                $transaction->description = "Kho #{$stock->id}: Nhập {$quantity} cái (VND)";
                $transaction->amount = -($quantity * $stock->cost_price);
                $transaction->paid_date = $stock->in_date;
                $transaction->cashier_id = $stock->updated_user_id;
                $transaction->save();

                $stock_transaction = new StockTransaction;
                $stock_transaction->stock_id = $stock->id;
                $stock_transaction->transaction_id = $transaction->id;
                $stock_transaction->save();
            }
        });
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
