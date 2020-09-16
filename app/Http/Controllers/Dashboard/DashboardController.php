<?php

namespace App\Http\Controllers\Dashboard;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

class DashboardController extends Controller
{
    public function index()
    {
        $loginPath = 'auth/login';
        if (!\Auth::check())
        {
            $curRoutePath = \Route::current()->parameters['any'] ?? null;
            if ($curRoutePath !== $loginPath)
            {
                return redirect()->route('dashboard.index', ['any' => $loginPath]);
            }
        }
        else
        {
            $curRoutePath = \Route::current()->parameters['any'] ?? null;
            if ($curRoutePath === $loginPath)
            {
                return redirect()->route('dashboard.index', ['any' => 'index']);
            }
        }

        return view('dashboard.app');
    }
}

