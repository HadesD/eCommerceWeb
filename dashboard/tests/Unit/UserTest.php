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
        global $argv, $argc;
        dd(bcrypt($argv[$argc-1]));
    }
}
