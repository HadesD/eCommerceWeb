<?php

namespace App;

use Illuminate\Contracts\Auth\MustVerifyEmail;
use Illuminate\Foundation\Auth\User as Authenticatable;
use Illuminate\Notifications\Notifiable;

interface Permissions
{
    const PERM_HOME_ACCESS = 1;
    const PERM_DASHBOARD_ACCESS = 2;
    const PERM_HOME_BUY = 4;
    const PERM_ = 8;

    public function hasPermissions(...$perms) : bool;
    public function addPermission(int $perm);
    public function removePermission(int $perm);
}

class User extends Authenticatable implements Permissions
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

    /**
     * Check if user has permssions
     *
     * @var int
     */
    public function hasPermissions(...$perms) : bool
    {
        foreach ($perms as $perm)
        {
            if (($this->permissions & $perm) !== $perm)
            {
                return false;
            }
        }

        return true;
    }

    public function addPermission(int $perm)
    {
        $this->permissions |= $perm;
    }

    public function removePermission(int $perm)
    {
        $this->permissions &= ~$perm;
    }

    static public function allPermissions()
    {
        return (new \ReflectionClass(Permissions::class))->getConstants();
    }
}

