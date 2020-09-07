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
        var_dump(\App\User::allPermissions());
        $this->assertTrue(in_array(pow(2, 0), \App\User::allPermissions()));
    }
}
