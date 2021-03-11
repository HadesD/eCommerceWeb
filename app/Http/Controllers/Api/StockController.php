<?php

namespace App\Http\Controllers\Api;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Stock;
use App\Models\Category;
use App\Models\StockCategory;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

class StockController extends Controller
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
                $category_id ? Category::find($category_id)->stocks->orderBy('in_date', 'DESC')->get()
                    : Stock::orderBy('in_date', 'DESC')->get()
            );
        }

        return new JsonResource(
            $category_id ? Category::find($category_id)->stocks->orderBy('in_date', 'DESC')->paginate()
                : Stock::orderBy('in_date', 'DESC')->paginate()
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

            $stock = new Stock;
            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->cost_price = ($request->cost_price < 0) ? 0 : $request->cost_price;
            $stock->updated_user_id = $request->user()->id;
            $stock->in_date = $request->in_date;
            $stock->note = $request->note;
            $stock->save();

            // EAV
            foreach ($request->categories_id as $category_id) {
                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ]);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($stock);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Stock $stock)
    {
        return new JsonResource($stock);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, Stock $stock)
    {
        try {
            DB::beginTransaction();

            if ($stock->products->count() && ($stock->cost_price !== $request->cost_price)) {
                throw new ApiErrorException('Can not change price of sold item');
            }

            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->quantity = ($request->quantity < 0) ? 0 : $request->quantity;
            $stock->cost_price = ($request->cost_price < 0) ? 0 : $request->cost_price;
            $stock->updated_user_id = $request->user()->id;
            $stock->in_date = $request->in_date;
            $stock->note = $request->note;
            $stock->save();

            // EAV
            foreach ($request->categories_id as $category_id) {
                $stock_category = StockCategory::where('stock_id', $stock->id)
                    ->where('category_id', $category_id)
                    ->first();
                if ($stock_category) {
                    continue;
                }

                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }

            StockCategory::where('stock_id', $stock->id)
                ->whereNotIn('category_id', $request->categories_id)
                ->delete();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ]);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($stock);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy(Stock $stock)
    {
        try {
            DB::beginTransaction();

            if ($stock->products) {
                throw new ApiErrorException('Không thể xoá sản phâm đã bán');
            }

            StockCategory::where('stock_id', $stock->id)
                ->delete();

            $stock->delete();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ]);
            }

            Log::error($e);

            return response(null, 500);
        }

        return response(null, 204);
    }
}
