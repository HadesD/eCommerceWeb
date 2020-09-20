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
}

