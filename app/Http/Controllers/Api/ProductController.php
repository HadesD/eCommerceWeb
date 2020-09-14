<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Http\Resources\Product as ProductResource;
use App\Models\Product;
use App\Models\Category;
use App\Models\ProductCategory;

use Illuminate\Support\Facades\Log;

class ProductController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index()
    {
        $query = \Request::all();

        $category_id = $query['category_id'] ?? 0;
        if ($category_id)
        {

        }

        return new ProductResource(Product::paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        try
        {
            \DB::beginTransaction();

            $product = new Product;
            $product->name = $request->name;
            $product->slug = $request->slug;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            $product->image_variant_id = 0;
            $product->save();

            // EAV
            foreach ($request->categories_id as $category_id)
            {
                // $product->categories[] = Category::find($category_id);

                $product_category = new ProductCategory;
                $product_category->product_id = $product->id;
                $product_category->category_id = $category_id;
                $product_category->save();
            }

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            throw new \RuntimeException($e);
        }

        return new ProductResource($product ?? []);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show($id)
    {
        return new ProductResource(Product::find($id));
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, $id)
    {
        try
        {
            \DB::beginTransaction();

            $product = Product::find($id);
            $product->name = $request->name;
            $product->slug = $request->slug;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            $product->image_variant_id = 0;
            $product->save();

            // EAV
            // foreach ($request->categories_id as $category_id)
            // {
                // $product->categories[] = Category::find($category_id);

            //     $product_category = new ProductCategory;
            //     $product_category->product_id = $product->id;
            //     $product_category->category_id = $category_id;
            //     $product_category->save();
            // }

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            throw new \RuntimeException($e);
        }

        return new ProductResource($product ?? []);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy($id)
    {
        return new ProductResource(Product::find($id)->delete() ? [
            'error_code' => 0,
        ] : []);
    }
}
