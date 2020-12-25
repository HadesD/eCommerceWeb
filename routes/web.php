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

Route::middleware(['auth:sanctum', 'verified', 'role.manager'])->get('/dashboard{any?}', function(){
    return view('dashboard.index');
})->where('any', '.*')->name('dashboard.index');

Route::name('web.')->group(function(){
    Route::get('/', [App\Http\Controllers\Web\WebController::class, 'index'])->name('index');
});

