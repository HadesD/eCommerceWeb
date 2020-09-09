<?php

namespace Tests\Unit;

use PHPUnit\Framework\TestCase;

class UserTest extends TestCase
{
    /**
     * A basic unit test example.
     *
     * @return void
     */
    public function testExample()
    {
        var_dump(\App\User::allRoles());
        $this->assertTrue(is_array(\App\User::allRoles()));
    }
}
