<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

interface Status
{
    const STS_DRAFT = 0;
    const STS_SELLING = 1;
    const STS_SOLDOUT = 2;
}

class Product extends Model
    implements Status
{
    use HasFactory;

    protected $fillable = ['name', 'description', 'detail', 'specification', 'slug', 'price'];

    public $categories = [];

    static public function allStatus()
    {
        return (new \ReflectionClass(Status::class))->getConstants();
    }

    // public function categories()
    // {
    //     return Category::whereHas('product_categories', function($query) {
    //         $query->where('product_id', $this->id);
    //     })->get();
    // }
}

