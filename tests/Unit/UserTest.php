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
        dd(bcrypt('123456'));
    }
}
