<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\Category;
use App\Models\Product;
use App\Models\ProductCategory;
use Illuminate\Http\Request;
use Illuminate\Http\Resources\Json\JsonResource;

class ProductController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $query = Product::query();

        $category_id = Category::query()
            ->where('slug', $request->category_slug)
            ->value('id');

        if ($category_id !== null) {
            $query->whereIn('id', function ($qW) use ($category_id) {
                $qW->select('product_id')
                    ->from((new ProductCategory())->getTable())
                    ->where('category_id', $category_id);
            });
        }

        return new JsonResource(
            $query->paginate(15)
        );
    }

    /**
     * Display the specified resource.
     *
     * @param  Product  $product
     * @return \Illuminate\Http\Response
     */
    public function show(Product $product)
    {
        return new JsonResource($product);
    }
}
