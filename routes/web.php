<?php

use Illuminate\Support\Facades\Auth;
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

Route::get('/login', [App\Http\Controllers\Auth\LoginController::class, 'index'])->name('login');
Route::post('/login', [App\Http\Controllers\Auth\LoginController::class, 'login']);
Route::post('/logout', [App\Http\Controllers\Auth\LoginController::class, 'logout']);

Route::prefix('/dashboard')->name('dashboard.')->group(function(){
    Route::get('/login', function () {
        if (Auth::check()) {
            return redirect()->route('dashboard.index');
        }
        return view('dashboard.index');
    })->name('login');

    Route::middleware(['auth:sanctum', 'verified', 'role.manager'])->get('/{any?}', function() {
        return view('dashboard.index');
    })->where('any', '.*')->name('index');
});

Route::name('web.')->group(function(){
    Route::get('/', [App\Http\Controllers\Web\WebController::class, 'index'])->name('index');
});

