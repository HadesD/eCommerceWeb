<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

class OrderProductStock extends Model
{
    use HasFactory;
    // use SoftDeletes;

    protected $appends = [
        'transactions',
    ];

    public function getTransactionsAttribute()
    {
        return Transaction::whereIn('id', function($query){
            $query->select('transaction_id')
                ->from((new OrderProductStockTransaction)->getTable())
                ->where('order_product_stock_id', $this->id);
        })->get();
    }
}
