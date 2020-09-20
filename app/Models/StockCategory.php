<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;

class StockCategory extends Model
{
    use HasFactory;

    protected $primaryKey = ['product_id', 'category_id'];
    public $incrementing = false;

    protected $fillable = ['product_id', 'category_id'];
    public $timestamps = false;

}
