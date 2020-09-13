<?php

namespace Tests\Unit;

use PHPUnit\Framework\TestCase;

use App\Models\Product;

class ProductTest extends TestCase
{
    /**
     * A basic unit test example.
     *
     * @return void
     */
    public function testExample()
    {
        parent::setUp();
        // $this->assertTrue(true);
        print_r(Product::allStatus());

        print_r(Product::find(1));
    }
}
