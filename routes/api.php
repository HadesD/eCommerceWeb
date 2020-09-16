<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| API Routes
|--------------------------------------------------------------------------
|
| Here is where you can register API routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| is assigned the "api" middleware group. Enjoy building your API!
|
*/

// Route::middleware('auth:sanctum')->get('/user', function (Request $request) {
//     return $request->user();
// });

Route::middleware('auth:sanctum')->group(function(){
    Route::apiResources([
        'stocks' => App\Http\Controllers\Api\StockController::class,
        'categories' => App\Http\Controllers\Api\CategoryController::class,
        'products' => App\Http\Controllers\Api\ProductController::class,
    ]);
});

