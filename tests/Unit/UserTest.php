<?php

namespace Tests\Unit;

use PHPUnit\Framework\TestCase;

use App\Models\User;

class UserTest extends TestCase
{
    /**
     * A basic unit test example.
     *
     * @return void
     */
    public function testExample()
    {
        var_dump(User::allRoles());
        $this->assertTrue(is_array(User::allRoles()));
    }
}
