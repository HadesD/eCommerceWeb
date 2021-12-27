<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class AlterColumnStatusIntoTableOrderProductStocks extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::table('order_product_stocks', function (Blueprint $table) {
            $table->unsignedTinyInteger('status')->after('stock_id')->nullable();
        });
    }

    /**
     * Reverse the migrations.
     *
     * @return void
     */
    public function down()
    {
        Schema::table('order_product_stocks', function (Blueprint $table) {
            $table->dropColumn('status');
        });
    }
}
