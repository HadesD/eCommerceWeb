<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class MigrateStockTable2 extends Migration
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
            $cleared = [];
            foreach ($allStocks as $stock)
            {
                if (isset($cleared[$stock->idi]) && ($cleared[$stock->idi] == $stock->cost_price))
                {
                    $stock->delete();
                    continue;
                }

                $stock->quantity = \App\Models\Stock::where('idi', $stock->idi)->where('cost_price', $stock->cost_price)->sum('quantity');
                $stock->save();
                $cleared[$stock->idi] = $stock->cost_price;
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
