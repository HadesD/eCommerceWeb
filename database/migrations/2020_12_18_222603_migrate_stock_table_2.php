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
        throw 1;
        Schema::table('stocks', function (Blueprint $table) {
            \DB::statement(
                'UPDATE stocks m_s SET quantity=(
                    SELECT SUM(s_s.quantity) FROM stocks s_s WHERE s_s.idi=m_s.idi AND s_s.cost_price=m_s.cost_price
                ) WHERE m_s.quantity>0 LIMIT 1;'
            );
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
