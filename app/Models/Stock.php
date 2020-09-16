<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface StockStatus
{
    const STS_AVAILABLE = 0;
    const STS_SOLD = 1;
    const STS_BROKEN = 2;
}

class Stock extends Model
    implements StockStatus
{
    use HasFactory, SoftDeletes;

    protected $fillable = ['name', 'idi', 'status', 'cost_price', 'updated_user_id', 'in_date'];
}

