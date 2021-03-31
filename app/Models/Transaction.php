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

    public function setPaidDateAttribute($value){
        $this->attributes['paid_date'] = Carbon::parse($value)->format('Y-m-d H:i:s');
    }

    /**
     * Prepare a date for array / JSON serialization.
     *
     * @param  \DateTimeInterface  $date
     * @return string
     **/
    // protected function serializeDate(DateTimeInterface $date)
    // {
    //     return $date->format('Y-m-d H:i:s');
    // }
}

