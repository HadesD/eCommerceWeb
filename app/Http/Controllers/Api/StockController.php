<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Http\Resources\Stock as StockResource;
use App\Models\Stock;

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
        return new StockResource(Stock::paginate());
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

            $num = $request->num ?? 1;

            for ($i = 0; $i < $num; $i++)
            {
                $stock = new Stock;
                $stock->name = $request->name;
                $stock->idi = $request->idi;
                $stock->status = $request->status;
                $stock->cost_price = $request->cost_price;
                $stock->updated_user_id = \Auth::user()->id;
                $stock->in_date = \Carbon\Carbon::parse($request->in_date)->format('Y-m-d H:i:s');
                $stock->note = $request->note;
                $stock->save();
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
            $stock->name = $request->name;
            $stock->idi = $request->idi;
            $stock->status = $request->status;
            $stock->cost_price = $request->cost_price;
            $stock->updated_user_id = \Auth::user()->id;
            $stock->in_date = \Carbon\Carbon::parse($request->in_date)->format('Y-m-d H:i:s');
            $stock->note = $request->note;
            $stock->save();

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
        if ($stock->product)
        {
            throw new \RuntimeException('Stock sold');
        }

        return new StockResource($stock->delete() ? [
            'error_code' => 0,
        ] : []);
    }
}
