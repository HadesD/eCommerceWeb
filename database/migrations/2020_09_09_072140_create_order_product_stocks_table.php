<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class CreateOrderProductStocksTable extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::create('order_product_stocks', function (Blueprint $table) {
            $table->bigIncrements('id');
            $table->unsignedBigInteger('order_product_id');
            $table->unsignedBigInteger('stock_id');
            $table->integer('amount');
            $table->timestamps();

            $table->unique(['order_product_id', 'stock_id']);
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::dropIfExists('order_product_stocks');
    }
}
