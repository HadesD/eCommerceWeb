<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Product;
use App\Models\Category;
use App\Models\ProductCategory;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

class ProductController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $category_id = $request->category_id ?? 0;
        if (isset($request->all)) {
            return new JsonResource(
                $category_id ? Category::find($category_id)->products->orderBy('updated_at', 'DESC')->get()
                    : Product::orderBy('updated_at', 'DESC')->get()
            );
        }

        return new JsonResource(
            $category_id ? Category::find($category_id)->products->orderBy('updated_at', 'DESC')->paginate()
                : Product::orderBy('updated_at', 'DESC')->paginate()
        );
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        try {
            DB::beginTransaction();

            $product = new Product;
            $product->name = $request->name;
            $product->slug = $request->slug;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            // $product->image_variant_id = 0;
            $product->save();

            // EAV
            foreach ($request->categories_id as $category_id) {
                $product_category = new ProductCategory;
                $product_category->product_id = $product->id;
                $product_category->category_id = $category_id;
                $product_category->save();
            }

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($product);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Product $product)
    {
        return new JsonResource($product);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, Product $product)
    {
        try {
            DB::beginTransaction();

            $product->name = $request->name;
            $product->slug = $request->slug;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            // $product->image_variant_id = 0;
            $product->save();

            // EAV
            foreach ($request->categories_id as $category_id) {
                $product_category = ProductCategory::where('product_id', $product->id)
                    ->where('category_id', $category_id)
                    ->first();
                if ($product_category) {
                    continue;
                }

                $product_category = new ProductCategory;
                $product_category->product_id = $product->id;
                $product_category->category_id = $category_id;
                $product_category->save();
            }

            ProductCategory::where('product_id', $product->id)
                ->whereNotIn('category_id', $request->categories_id)
                ->delete();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($product);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy(Product $product)
    {
        try {
            DB::beginTransaction();

            ProductCategory::where('product_id', $product->id)
                ->delete();

            $product->delete();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            Log::error($e);

            return response(null, 500);
        }

        return response(null, 204);
    }
}
