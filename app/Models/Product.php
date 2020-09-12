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

class Product extends Model implements Status
{
    use HasFactory;

    protected $fillable = ['name', 'description', 'detail', 'specification', 'slug', 'price'];

    static public function allStatus()
    {
        return (new \ReflectionClass(Status::class))->getConstants();
    }
}

