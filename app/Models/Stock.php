<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface StockStatus
{
    const STS_AVAILABLE = 0;
    const STS_SOLD = 1;
    const STS_BROKEN = 2;
}

class Stock extends Model
    implements StockStatus
{
    use HasFactory, SoftDeletes;

    protected $fillable = ['name', 'idi', 'status', 'cost_price', 'updated_user_id', 'in_date'];

    protected $appends = [
        'product',
        'categories',
    ];

    public function getProductAttribute(){
        return Product::where('id', function($query){
            $query->select('product_id')
                  ->from(with(new OrderProduct)->getTable())
                  ->where('id', function($q1){
                      $q1->select('order_product_id')
                         ->from(with(new OrderProductStock)->getTable())
                         ->where('stock_id', $this->id);
                  });
        })->first();
    }

    public function getCategoriesAttribute()
    {
        return Category::whereIn('id', function($query){
            $query->select('category_id')
                  ->from(with(new StockCategory)->getTable())
                  ->where('stock_id', $this->id);
        })->get();
    }
}

