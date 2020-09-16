<?php

use Illuminate\Support\Facades\Route;

/*
|--------------------------------------------------------------------------
| Web Routes
|--------------------------------------------------------------------------
|
| Here is where you can register web routes for your application. These
| routes are loaded by the RouteServiceProvider within a group which
| contains the "web" middleware group. Now create something great!
|
*/

Route::prefix('dashboard')->name('dashboard.')->group(function(){
    Route::get('{any?}', [App\Http\Controllers\Dashboard\DashboardController::class, 'index'])->where('any', '.*');
});

Route::name('web.')->group(function(){
    // Route::get('{any?}', 'DashboardController@index')->where('any', '.*');
});

// Route::middleware(['auth:sanctum', 'verified'])->get('/dashboard', function () {
//     return Inertia\Inertia::render('Dashboard');
// })->name('dashboard');
