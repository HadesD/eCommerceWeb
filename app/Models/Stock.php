<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

use Carbon\Carbon;
use DateTimeInterface;

class Stock extends Model
{
    use HasFactory, SoftDeletes;

    protected $fillable = [
        'name', 'idi', 'cost_price', 'updated_user_id', 'in_date'
    ];

    protected $appends = [
        'product',
        'categories',
    ];

    /**
     * Prepare a date for array / JSON serialization.
     *
     * @param  \DateTimeInterface  $date
     * @return string
     **/
    // protected function serializeDate(DateTimeInterface $date)
    // {
    //     return $date->format('Y-m-d H:i:s');
    // }

    public function setInDateAttribute($value){
        $this->attributes['in_date'] = Carbon::parse($value)->format('Y-m-d H:i:s');
    }

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

