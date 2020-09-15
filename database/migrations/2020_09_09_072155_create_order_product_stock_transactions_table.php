<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class CreateOrderProductStockTransactionsTable extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::create('order_product_stock_transactions', function (Blueprint $table) {
            $table->unsignedBigInteger('order_product_stock_id');
            $table->unsignedBigInteger('transaction_id');
            // $table->timestamps();
            $table->primary(['order_product_stock_id', 'transaction_id'], 'PK_opst_id_unique');
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::dropIfExists('order_product_stock_transactions');
    }
}
