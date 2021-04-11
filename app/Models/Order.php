<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface OrderStatus
{
    const STS_PROCESSING = 0;
    const STS_CANCELED = 10;
    const STS_PAID = 50;
    const STS_PAYING = 51;
    const STS_SHIPPING = 100;
    const STS_COMPLETED = 200;
}

class Order extends Model
    implements OrderStatus
{
    use HasFactory, SoftDeletes;

    protected $fillable = ['customer_id', 'note'];

    protected $appends = [
        // 'order_products',
        // 'transactions', // Addon fee,..
        // 'customer',
    ];

    public function getNeedPaidTotalAttribute() {
        $total = 0;

        foreach ($this->order_products as $op) {
            foreach ($op->order_product_stocks as $ops) {
                if ($ops->status === OrderProductStock::STS_SOLD) {
                    $total += $ops->amount;
                }
            }
        }

        return $total;
    }

    public function getStockCostTotalAttribute()
    {
        $total = 0;

        foreach ($this->order_products as $op) {
            foreach ($op->order_product_stocks as $ops) {
                if ($ops->status === OrderProductStock::STS_SOLD) {
                    $total += $ops->stock->cost_price;
                }
            }
        }

        return $total;
    }

    public function getEarnedTotalAttribute()
    {
        $total = 0;

        foreach ($this->transactions as $tnx) {
            $total += $tnx->amount;
        }

        foreach ($this->order_products as $op) {
            foreach ($op->order_product_stocks as $ops) {
                foreach ($ops->transactions as $tnx) {
                    $total += $tnx->amount;
                }
            }
        }

        return $total;
    }

    public function getCustomerAttribute()
    {
        return User::find($this->customer_id);
    }

    public function getUpdatedUserAttribute()
    {
        return User::find($this->updated_user_id);
    }

    public function getOrderProductsAttribute()
    {
        return OrderProduct::where('order_id', $this->id)->get();
    }

    public function getTransactionsAttribute()
    {
        return Transaction::whereIn('id', function($query){
            $query->select('transaction_id')
                  ->from((new OrderTransaction)->getTable())
                  ->where('order_id', $this->id);
        })->get();
    }
}
