<?php

namespace App\Models;

// use Illuminate\Contracts\Auth\MustVerifyEmail;
use Illuminate\Database\Eloquent\Factories\HasFactory;
use Illuminate\Foundation\Auth\User as Authenticatable;
use Illuminate\Notifications\Notifiable;
use Laravel\Sanctum\HasApiTokens;

interface UserRole
{
    const ROLE_USER_BLOCKED = 0;
    const ROLE_USER_UNACTIVE = 10;
    const ROLE_USER_NORMAL = 50;
    const ROLE_ADMIN_MANAGER = 100;
    const ROLE_ADMIN_SUB_MASTER = 190;
    const ROLE_ADMIN_MASTER = 200;
}

class User extends Authenticatable
    implements UserRole
{
    use HasApiTokens;
    use HasFactory;
    use Notifiable;

    /**
     * The attributes that are mass assignable.
     *
     * @var array
     */
    protected $fillable = [
        'name',
        'email',
        'password',
        'role',
        'phone',
        'sns_info',
    ];

    /**
     * The attributes that should be hidden for arrays.
     *
     * @var array
     */
    protected $hidden = [
        'password',
        'remember_token',
    ];

    /**
     * The attributes that should be cast to native types.
     *
     * @var array
     */
    protected $casts = [
        'email_verified_at' => 'datetime',
        'sns_info' => 'object',
    ];

    public function setSnsInfoAttribute($value)
    {
        $this->attributes['sns_info'] = json_encode($value);
    }

    public function hasPermission(int $perm) : bool
    {
        return ($this->role >= $perm);
    }

    static public function allRoles()
    {
        return (new \ReflectionClass(UserRole::class))->getConstants();
    }
}
