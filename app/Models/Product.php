<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface ProductStatus
{
    const STS_DRAFT = 0;
    const STS_SELLING = 1;
    const STS_SOLDOUT = 2;
}

class Product extends Model
    implements ProductStatus
{
    use HasFactory, SoftDeletes;

    protected $fillable = ['name', 'description', 'detail', 'specification', 'slug', 'price'];

    protected $appends = [
        'categories',
    ];

    static public function allStatus()
    {
        return (new \ReflectionClass(ProductStatus::class))->getConstants();
    }

    public function getCategoriesAttribute()
    {
        return Category::whereIn('id', function($query){
            $query->select('category_id')
                  ->from(with(new ProductCategory)->getTable())
                  ->where('product_id', $this->id);
        })->get();
    }
}

