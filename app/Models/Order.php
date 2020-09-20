<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

class Order extends Model
{
    use HasFactory, SoftDeletes;

    protected $fillable = ['customer_id', 'note'];

    protected $appends = [
        'order_products',
        'transactions', // Addon fee,..
    ];

    public function getOrderProductsAttribute()
    {
        return OrderProduct::where('order_id', $this->id)->get();
    }

    public function getTransactionsAttribute()
    {
        return Transaction::where('id', function($query){
            $query->select('transaction_id')
                  ->from((new OrderTransaction)->getTable())
                  ->where('order_id', $this->id);
        })->get();
    }
}
