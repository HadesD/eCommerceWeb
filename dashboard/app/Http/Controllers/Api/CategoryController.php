<?php

namespace App\Http\Controllers\Api;

use App\Http\Controllers\Controller;
use App\Models\Category;
use Illuminate\Http\Request;
use Illuminate\Http\Resources\Json\JsonResource;

class CategoryController extends Controller
{
    public function index(Request $request)
    {
        $category = Category::where('slug', $request->parent_slug)->first();

        $query = Category::query();

        if ($category) {
            $query->where('parent_id', $category->id);
        }

        return [
            'name' => $category ? $category->name : null,
            'data' => $query->get(),
        ];
    }
}
