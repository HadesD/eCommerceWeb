<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface PaymentMethod
{
    const PM_ONCE = 1;
    const PM_INSTALMENT = 2; // Tra gop
}

class OrderProduct extends Model
    implements PaymentMethod
{
    use HasFactory;
    // use SoftDeletes;

    protected $appends = [
        'product',
        'order_product_stocks',
    ];

    public function getProductAttribute()
    {
        return Product::where('id', $this->product_id)->first();
    }

    public function getOrderProductStocksAttribute()
    {
        return OrderProductStock::where('order_product_id', $this->id)->get();
    }
}
