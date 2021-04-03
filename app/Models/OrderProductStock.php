<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
// use Illuminate\Database\Eloquent\SoftDeletes;

interface OrderProductStockStatus
{
    const STS_SOLD = 1;
    const STS_REFUND = 2;
}

class OrderProductStock extends Model
    implements OrderProductStockStatus
{
    use HasFactory;
    // use SoftDeletes;

    protected $appends = [
        'transactions',
        'stock',
    ];

    public function getStockAttribute()
    {
        return Stock::where('id', $this->stock_id)->first();
    }

    public function getTransactionsAttribute()
    {
        return Transaction::whereIn('id', function($query){
            $query->select('transaction_id')
                ->from((new OrderProductStockTransaction)->getTable())
                ->where('order_product_stock_id', $this->id);
        })->get();
    }
}
