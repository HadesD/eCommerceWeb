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

        $ids = $request->ids;

        if (is_array($ids)) {
            $query->whereIn('id', $ids);

            $query = $query->get();
        } else {
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

            if ($priceRange = $request->price_range) {
                if (count($priceRange) === 2) {
                    $query->where('price', '>=', $priceRange[0])
                        ->where('price', '<=', $priceRange[1]);
                }
            }

            if ($sortBy = $request->sort_by) {
                $order = substr($sortBy, 0, 1) === '+' ? 'ASC' : 'DESC';
                $query->orderBy(substr($sortBy, 1), $order);
            }

            $query = $query->paginate(16);
        }

        return new JsonResource($query);
    }

    /**
     * Display the specified resource.
     *
     * @param  string  $slug || $id
     * @return \Illuminate\Http\Response
     */
    public function show($p)
    {
        return new JsonResource(Product::find($p) ?? Product::query()->where('slug', $p)->first());
    }
}
