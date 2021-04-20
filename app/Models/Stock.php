<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

class Stock extends Model
{
    use HasFactory, SoftDeletes;

    protected $fillable = [
        'name', 'idi', 'cost_price', 'updated_user_id', 'in_date'
    ];

    public function getCostTotalAttribute()
    {
        $cost_total = 0;

        foreach ($this->transactions as $tnx) {
            $cost_total += $tnx->amount;
        }

        return $cost_total;
    }

    public function getProductsAttribute(){
        return Product::whereIn('id', function($query){
            $query->select('product_id')
                  ->from(with(new OrderProduct)->getTable())
                  ->whereIn('id', function($q1){
                      $q1->select('order_product_id')
                         ->from(with(new OrderProductStock)->getTable())
                         ->where('stock_id', $this->id);
                  });
        })->get();
    }

    public function getCategoriesAttribute()
    {
        return Category::whereIn('id', function($query){
            $query->select('category_id')
                  ->from(with(new StockCategory)->getTable())
                  ->where('stock_id', $this->id);
        })->get();
    }

    public function getTransactionsAttribute()
    {
        return Transaction::whereIn('id', function($query) {
            $query->select('transaction_id')
                ->from(with(new StockTransaction)->getTable())
                ->where('stock_id', $this->id);
        })->get();
    }

    public function getUpdatedUserAttribute()
    {
        return User::find($this->updated_user_id);
    }

    public function getTesterAttribute()
    {
        return User::find($this->tester_id);
    }

    public function getOrdersHistoryAttribute()
    {
        return Order::whereIn('id', function ($query) {
            $query->select('order_id')
                ->from(with(new OrderProduct)->getTable())
                ->whereIn('id', function ($query) {
                    $query->select('order_product_id')
                        ->from(with(new OrderProductStock)->getTable())
                        ->where('stock_id', $this->id);
                });
        })->get();
    }
}
