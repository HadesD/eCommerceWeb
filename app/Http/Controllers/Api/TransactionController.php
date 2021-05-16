<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\Transaction;
use Illuminate\Http\Request;
use Illuminate\Http\Resources\Json\JsonResource;

class TransactionController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index(Request $request)
    {
        $transactionQuery = new Transaction;

        foreach (['description', 'id'] as $value) {
            if (isset($request->{$value})) {
                $transactionQuery = $transactionQuery->where($value, 'LIKE', '%'.(is_array($request->{$value}) ? $request->{$value}[0] : $request->{$value}).'%');
            }
        }

        if (isset($request->paid_date)) {
            $transactionQuery = $transactionQuery
                ->where('paid_date', '>=', date('Y-m-d 00:00:00', strtotime($request->paid_date[0])))
                ->where('paid_date', '<=', date('Y-m-d 23:59:59', strtotime($request->paid_date[1])));
        }

        if (isset($request->sort_by)) {
            foreach (explode(',', $request->sort_by) as $sorter) {
                $col = substr($sorter, 1);
                $sortType = ($sorter[0] === '-') ? 'DESC' : 'ASC';
                $transactionQuery = $transactionQuery->orderBy($col, $sortType);
            }
        } else {
            $transactionQuery = $transactionQuery->orderBy('paid_date', 'DESC');
        }


        if (isset($request->download)) {
            $transactionQuery = $transactionQuery->get();
            $transactionQuery->append(['type']);

            switch ($request->download) {
                case 'csv': {
                    $csv = \League\Csv\Writer::createFromFileObject(new \SplTempFileObject);
                    $colNameRow = array_keys($transactionQuery[0]->toArray());
                    foreach ($colNameRow as &$colName) {
                        $colName = __('csv.' . $colName);
                    }
                    $csv->insertOne($colNameRow);

                    foreach ($transactionQuery as $transaction) {
                        $csv->insertOne($transaction->toArray());
                    }

                    return response((string)$csv, 200, [
                        'Content-Type' => 'text/csv',
                        'Content-Transfer-Encoding' => 'binary',
                        'Content-Disposition' => 'attachment; filename="'.parse_url(config('app.url'))['host'].'_transactions_'.date('Y-m-d_H-i-s').'.csv"',
                    ]);
                } break;
            }
        }

        $transactionQuery = $transactionQuery->paginate();

        $transactionQuery->append(['stock', 'order', 'cashier', 'type']);

        return new JsonResource($transactionQuery);
    }

    /**
     * Store a newly created resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @return \Illuminate\Http\Response
     */
    public function store(Request $request)
    {
        //
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show($id)
    {
        //
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
        //
    }

    /**
     * Remove the specified resource from storage.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function destroy($id)
    {
        //
    }
}
