<?php

namespace App\Http\Controllers\Api\Dashboard;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Product;
use App\Models\Category;
use App\Models\Image;
use App\Models\ProductCategory;
use App\Models\ProductImage;
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
        $productQuery = isset($request->category_id) ? Category::find($request->category_id)->products
                : (new Product);

        if (isset($request->status)) {
            $productQuery = $productQuery->whereIn('status', $request->status);
        }

        foreach (['name', 'id'] as $value) {
            if (isset($request->{$value})) {
                $productQuery = $productQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        if (isset($request->sort_by)) {
            foreach (explode(',', $request->sort_by) as $sorter) {
                $col = substr($sorter, 1);
                $sortType = ($sorter[0] === '-') ? 'DESC' : 'ASC';
                $productQuery = $productQuery->orderBy($col, $sortType);
            }
        }

        $productQuery = $productQuery->orderBy('created_at', 'DESC');

        return new JsonResource($productQuery->paginate());
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        $this->validate($request, [
            'name' => 'required',
            'price' => 'required',
            'status' => 'required',
            'categories_id' => 'required|array',
        ]);

        try {
            DB::beginTransaction();

            $product = new Product;
            $product->name = $request->name;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            // $product->image_variant_id = 0;
            $product->save();

            // Categories
            foreach ($request->categories_id as $category_id) {
                $product_category = new ProductCategory;
                $product_category->product_id = $product->id;
                $product_category->category_id = $category_id;
                $product_category->save();
            }

            // Images
            foreach ($request->images as $_image) {
                $image = null;
                if (isset($_image['id'])) {
                    $image = Image::find($_image['id']);
                }
                if (!$image) {
                    $image = new Image;
                    $image->url = $_image['url'];
                    $image->save();
                }

                $productImage = new ProductImage;
                $productImage->image_id = $image->id;
                $productImage->product_id = $product->id;
                $productImage->save();
            }

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            $error_code = 500;

            if (!$e instanceof ApiErrorException) {
                Log::error($e);

                $message = 'Lỗi hệ thống';
            } else {
                $message = $e->getMessage();
                $error_code = 400;
            }

            return response(['message' => $message], $error_code);
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
        $this->validate($request, [
            'name' => 'required',
            'price' => 'required',
            'status' => 'required',
            'categories_id' => 'required|array',
        ]);

        try {
            DB::beginTransaction();

            $product->name = $request->name;
            $product->price = $request->price;
            $product->status = (int)$request->status;
            $product->description = $request->description;
            $product->detail = $request->detail;
            $product->specification = $request->specification;
            // $product->image_variant_id = 0;
            $product->save();

            // Categories
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

            // Images
            $images_id = [];
            foreach ($request->images as $_image) {
                $image = null;
                if (isset($_image['id'])) {
                    $image = Image::find($_image['id']);
                }
                if (!$image) {
                    $image = new Image;
                    $image->url = $_image['url'];
                    $image->save();
                }

                $images_id[] = $image->id;

                if (ProductImage::where('product_id', $product->id)->where('image_id', $image->id)->exists()) {
                    continue;
                }

                $productImage = new ProductImage();
                $productImage->image_id = $image->id;
                $productImage->product_id = $product->id;
                $productImage->save();
            }
            ProductImage::where('product_id', $product->id)
                ->whereNotIn('image_id', $images_id)
                ->delete();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            $error_code = 500;

            if (!$e instanceof ApiErrorException) {
                Log::error($e);

                $message = 'Lỗi hệ thống';
            } else {
                $message = $e->getMessage();
                $error_code = 400;
            }

            return response(['message' => $message], $error_code);
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
        //
    }
}
