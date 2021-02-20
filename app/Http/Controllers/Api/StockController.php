<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Http\Resources\Stock as StockResource;
use App\Models\Stock;
use App\Models\Category;
use App\Models\StockCategory;

use Illuminate\Support\Facades\Log;

class StockController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index()
    {
        $params = \Request::all();
        $category_id = $params['category_id'] ?? 0;
        if (isset($params['all']))
        {
            return new StockResource($category_id ? Category::find($category_id)->stocks->orderBy('in_date', 'DESC')->get() : Stock::orderBy('in_date', 'DESC')->get());
        }

        return new StockResource($category_id ? Category::find($category_id)->stocks->orderBy('in_date', 'DESC')->paginate() : Stock::orderBy('in_date', 'DESC')->paginate());
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
            foreach ($request->categories_id as $category_id)
            {
                $stock_category = new StockCategory;
                $stock_category->stock_id = $stock->id;
                $stock_category->category_id = $category_id;
                $stock_category->save();
            }

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            Log::error($e);

            throw new \RuntimeException($e);
        }

        return new StockResource($stock);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show($id)
    {
        return new StockResource(Stock::find($id));
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

            $stock = Stock::find($id);

            if ($stock->products->count() && ($stock->cost_price !== $request->cost_price))
            {
                throw new \RuntimeException('Can not change price of sold item');
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
            foreach ($request->categories_id as $category_id)
            {
                $stock_category = StockCategory::where('stock_id', $stock->id)
                    ->where('category_id', $category_id)
                    ->first();
                if ($stock_category)
                {
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

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            Log::error($e);

            throw new \RuntimeException($e);
        }

        return new StockResource($stock);
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy($id)
    {
        $stock = Stock::find($id);

        if ($stock->products)
        {
            throw new \RuntimeException('Stock sold');
        }

        try
        {
            \DB::beginTransaction();

            StockCategory::where('stock_id', $stock->id)
                ->delete();

            $stock->delete();

            \DB::commit();
        }
        catch(\Throwable $e)
        {
            \DB::rollback();

            Log::error($e);

            throw new \RuntimeException($e);
        }

        return [
            'error_code' => 0,
        ];
    }
}

