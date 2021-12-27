<?php

namespace App\Http\Controllers\Api\Dashboard;

use App\Exceptions\ApiErrorException;
use App\Http\Controllers\Controller;
use Illuminate\Http\Request;

use App\Models\Category;
use Illuminate\Http\Resources\Json\JsonResource;
use Illuminate\Support\Facades\DB;
use Illuminate\Support\Facades\Log;

class CategoryController extends Controller
{
    /**
     * Display a listing of the resource.
     *
     * @return \Illuminate\Http\Response
     */
    public function index()
    {
        return new JsonResource(Category::all());
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
            'slug' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $category = new Category;
            $category->name = $request->name;
            $category->slug = $request->slug;
            $category->parent_id = $request->parent_id;
            $category->description = $request->description;
            $category->save();

            DB::commit();
        } catch (\Throwable $e) {
            DB::rollBack();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($category);
    }

    /**
     * Display the specified resource.
     *
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function show(Category $category)
    {
        return new JsonResource($category);
    }

    /**
     * Update the specified resource in storage.
     *
     * @param  \Illuminate\Http\Request  $request
     * @param  int  $id
     * @return \Illuminate\Http\Response
     */
    public function update(Request $request, Category $category)
    {
        $this->validate($request, [
            'name' => 'required',
            'slug' => 'required',
        ]);

        try {
            DB::beginTransaction();

            $category->name = $request->name;
            $category->slug = $request->slug;
            $category->parent_id = $request->parent_id;
            $category->description = $request->description;
            $category->save();

            DB::commit();
        } catch(\Throwable $e) {
            DB::rollback();

            if ($e instanceof ApiErrorException) {
                return response([
                    'message' => $e->getMessage()
                ], 400);
            }

            Log::error($e);

            return response(null, 500);
        }

        return $this->show($category);
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
