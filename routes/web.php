<?php

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

Route::namespace('Dashboard')->prefix('dashboard')->name('dashboard.')->group(function(){
    Route::get('{any?}', 'DashboardController@index')->where('any', '.*');
});

