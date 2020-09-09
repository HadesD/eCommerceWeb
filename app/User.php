<?php

namespace App;

use Illuminate\Contracts\Auth\MustVerifyEmail;
use Illuminate\Foundation\Auth\User as Authenticatable;
use Illuminate\Notifications\Notifiable;

/**
 * Greater is better
 */
interface Role
{
    const ROLE_USER_BLOCKED = -100;
    const ROLE_USER_UNACTIVE = 0;
    const ROLE_USER_NORMAL = 10;
    const ROLE_ADMIN_MANAGER = 50;
    const ROLE_ADMIN_MASTER = 100;
}

class User extends Authenticatable implements Role
{
    use Notifiable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'name', 'email', 'password', 'permissions',
    ];

    /**
     * The attributes that should be hidden for arrays.
     *
     * @var array
     */
    protected $hidden = [
        'password', 'remember_token',
    ];

    /**
     * The attributes that should be cast to native types.
     *
     * @var array
     */
    protected $casts = [
        'email_verified_at' => 'datetime',
    ];

    public function hasPermission(int $perm) : bool
    {
        return ($this->role >= $perm);
    }

    static public function allRoles()
    {
        return (new \ReflectionClass(Role::class))->getConstants();
    }
}

