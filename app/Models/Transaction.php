<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

use Carbon\Carbon;
use DateTimeInterface;

interface TransactionStatus
{
    const STS_PENDING = 0;
    const STS_COMPLETED = 1;
    const STS_CANCELED = -1;
}

class Transaction extends Model
    implements TransactionStatus
{
    use HasFactory, SoftDeletes;

    public function setPaidDateAttribute($value)
    {
        $this->attributes['paid_date'] = Carbon::parse($value)->format('Y-m-d H:i:s');
    }

    public function getStockAttribute()
    {
        return Stock::where('id', function($query) {
            $query->select('stock_id')
                ->from((new StockTransaction)->getTable())
                ->where('transaction_id', $this->id);
        })->first();
    }

    public function getOrderAttribute()
    {
        $order = Order::where('id', function($query) {
            $query->select('order_id')
                ->from((new OrderTransaction)->getTable())
                ->where('transaction_id', $this->id);
        })->first();

        if (!$order) {
            $order = Order::where('id', function($query) {
                $query->select('order_id')
                    ->from((new OrderProduct)->getTable())
                    ->where('id', function($query) {
                        $query->select('order_product_id')
                            ->from((new OrderProductStock)->getTable())
                            ->where('id', function($query) {
                                $query->select('order_product_stock_id')
                                    ->from((new OrderProductStockTransaction)->getTable())
                                    ->where('transaction_id', $this->id);
                            });
                    });
            })->first();
        }

        return $order;
    }
}

