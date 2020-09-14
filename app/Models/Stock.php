<?php

namespace App\Models;

use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Database\Eloquent\Model;
use Illuminate\Database\Eloquent\SoftDeletes;

interface Status
{
    const STS_ = 1;
}

class Stock extends Model
    implements Status
{
    use HasFactory, SoftDeletes;
}

