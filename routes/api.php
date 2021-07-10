<?php

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Route;

use App\Http\Controllers\Custom\GitHubWebhookController;

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

Route::post('/webhooks/github', [GitHubWebhookController::class, 'index']);

Route::middleware('auth:sanctum')->get('/user', function (Request $request) {
    return $request->user();
});

Route::prefix('/dashboard')->group(function () {
    Route::middleware(['auth:sanctum', 'role.manager'])->group(function () {
        Route::apiResources([
            'orders' => App\Http\Controllers\Api\OrderController::class,
            'stocks' => App\Http\Controllers\Api\StockController::class,
            'categories' => App\Http\Controllers\Api\CategoryController::class,
            'products' => App\Http\Controllers\Api\ProductController::class,
            'users' => App\Http\Controllers\Api\UserController::class,
            'statistics' => App\Http\Controllers\Api\StatisticController::class,
            'transactions' => App\Http\Controllers\Api\TransactionController::class,
        ]);
    });
});
