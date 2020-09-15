<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface Status
{
    const STS_PENDING = 0;
    const STS_COMPLETED = 1;
    const STS_CANCELED = -1;
}

class Transaction extends Model
    implements Status
{
    use HasFactory, SoftDeletes;
}
