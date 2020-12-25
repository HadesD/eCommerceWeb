<?php
namespace App\Http\Controllers;

use Illuminate\Http\Request;
use Illuminate\Support\Facades\Auth;

use App\Providers\RouteServiceProvider;

class LoginController extends Controller
{
    protected $redirectTo = RouteServiceProvider::HOME;

    /**
     * Handle an authentication attempt.
     *
     * @param  \Illuminate\Http\Request $request
     * @return \Illuminate\Http\Response
     */
    public function authenticate(Request $request)
    {
        $credentials = $request->only('email', 'password');

        if (Auth::attempt($credentials, $remember->remember ?? true))
        {
            $request->session()->regenerate();

            if ($user->hasPermission(\App\Models\User::ROLE_ADMIN_MANAGER))
            {
                // return redirect()->route('dashboard.index');
            }

            // return redirect($this->redirectTo());
        }
        return redirect()->route('dashboard.index');

        return back()->withErrors([
            'email' => 'The provided credentials do not match our records.',
        ]);
    }
}
