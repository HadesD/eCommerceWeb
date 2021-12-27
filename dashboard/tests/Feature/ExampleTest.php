<?php

namespace Tests\Feature;

use Illuminate\Foundation\Testing\RefreshDatabase;
use Tests\TestCase;

class ExampleTest extends TestCase
{
    /**
     * A basic test example.
     *
     * @return void
     */
    public function testBasicTest()
    {
        dd(\Carbon\Carbon::parse('2020-09-10 10:10:10')->format('Y-m-d H:i:s'));
    }
}
