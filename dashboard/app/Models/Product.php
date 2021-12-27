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

    protected $appends = [
        'categories', 'images'
    ];

    public function getCategoriesAttribute()
    {
        return Category::whereIn('id', function($query){
            $query->select('category_id')
                  ->from(with(new ProductCategory)->getTable())
                  ->where('product_id', $this->id);
        })->get();
    }

    /**
     * @return Image[]
     */
    public function getImagesAttribute()
    {
        return Image::whereIn('id', function ($query) {
            $query->select('image_id')
                ->from(with(new ProductImage())->getTable())
                ->where('product_id', $this->id);
        })->get();
    }
}

