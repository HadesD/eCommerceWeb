<?php

namespace App\Http\Middleware;

use Closure;
use Illuminate\Http\Request;

use App\Models\User;

class UserHasMasterAdminPermission
{
    /**
     * Handle an incoming request.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  \Closure  $next
     * @return mixed
     */
    public function handle(Request $request, Closure $next)
    {
        if (!$request->user()->hasPermission(User::ROLE_ADMIN_MASTER))
        {
            return redirect()->route('login');
        }

        return $next($request);
    }
}
