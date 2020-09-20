<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class Category extends Model
{
    use HasFactory;

    protected $fillable = ['name', 'description', 'parent_id', 'slug'];
    public $timestamps = false;

    public function getProductsAttribute()
    {
        return Product::whereIn('id', function($query){
            $query->select('product_id')
                  ->from(with(new ProductCategory)->getTable())
                  ->where('category_id', $this->id);
        });
    }

    public function getStocksAttribute()
    {
        return Stock::whereIn('id', function($query){
            $query->select('stock_id')
                  ->from(with(new StockCategory)->getTable())
                  ->where('category_id', $this->id);
        });
    }
}

