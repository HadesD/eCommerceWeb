<?php

namespace App\Http\Controllers\Dashboard;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Auth;
use Illuminate\Support\Facades\Route;

use App\Models\User;

class DashboardController extends Controller
{
    public function index()
    {
        $loginPath = 'auth/login';
        $curRoutePath = Route::current()->parameters['any'] ?? null;

        if (!Auth::check() || !Auth::user()->hasPermission(User::ROLE_ADMIN_MANAGER))
        {
            Auth::logout();

            if ($curRoutePath !== $loginPath)
            {
                return redirect()->route('dashboard.index', ['any' => $loginPath]);
            }
        }
        else
        {
            if ($curRoutePath === $loginPath)
            {
                return redirect()->route('dashboard.index', ['any' => 'index']);
            }
        }

        return view('dashboard.index');
    }
}

