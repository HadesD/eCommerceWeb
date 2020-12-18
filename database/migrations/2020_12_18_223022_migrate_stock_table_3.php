<?php

use Illuminate\Database\Migrations\Migration;
use Illuminate\Database\Schema\Blueprint;
use Illuminate\Support\Facades\Schema;

class MigrateStockTable3 extends Migration
{
    /**
     * Run the migrations.
     *
     * @return void
     */
    public function up()
    {
        Schema::table('stocks', function (Blueprint $table) {
            \DB::statement(
                'UPDATE stocks m_s SET deleted_at=NOW() WHERE quantity < (
                    SELECT MAX(quantity) FROM stocks s_s WHERE s_s.idi=m_s.idi AND s_s.cost_price=m_s.cost_price
                ) AND m_s.deleted_at IS NULL;'
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
