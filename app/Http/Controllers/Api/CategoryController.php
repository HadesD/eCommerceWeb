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
        $parent_id = Category::query()
            ->where('slug', $request->parent_slug)
            ->value('id');

        return new JsonResource(
            $parent_id ? Category::query()
                ->where('parent_id', $parent_id)
                ->get()
                : Category::all()
        );
    }
}
