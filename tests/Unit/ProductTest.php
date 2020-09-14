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
        print_r(Product::allStatus());

        $this->get('/api/products?category_id=4');
    }
}
