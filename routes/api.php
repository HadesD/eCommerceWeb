<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

use App\Http\Controllers\GitHubWebhookController;

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

Route::middleware('auth:sanctum')->get('/user', function (Request $request) {
    return $request->user();
});

Route::apiResources([
    // 'orders' => App\Http\Controllers\Api\Dashboard\OrderController::class,
    // 'stocks' => App\Http\Controllers\Api\Dashboard\StockController::class,
    'categories' => App\Http\Controllers\Api\CategoryController::class,
    // 'products' => App\Http\Controllers\Api\Dashboard\ProductController::class,
    // 'users' => App\Http\Controllers\Api\Dashboard\UserController::class,
    // 'statistics' => App\Http\Controllers\Api\Dashboard\StatisticController::class,
    // 'transactions' => App\Http\Controllers\Api\Dashboard\TransactionController::class,
]);

Route::prefix('/dashboard')->middleware(['auth:sanctum', 'role.manager'])->group(function () {
    Route::apiResources([
        'orders' => App\Http\Controllers\Api\Dashboard\OrderController::class,
        'stocks' => App\Http\Controllers\Api\Dashboard\StockController::class,
        'categories' => App\Http\Controllers\Api\Dashboard\CategoryController::class,
        'products' => App\Http\Controllers\Api\Dashboard\ProductController::class,
        'users' => App\Http\Controllers\Api\Dashboard\UserController::class,
        'statistics' => App\Http\Controllers\Api\Dashboard\StatisticController::class,
        'transactions' => App\Http\Controllers\Api\Dashboard\TransactionController::class,
    ]);
});

Route::post('/webhooks/github', [GitHubWebhookController::class, 'index']);
