<?php

namespace App\Http\Controllers\Custom;

use App\Http\Controllers\Controller;
use Illuminate\Http\Request;
use Illuminate\Support\Facades\Log;

class GitHubWebhookController extends Controller
{
    public function index(Request $request)
    {
        $branchRef = $request->ref;
        if ($branchRef !== 'refs/heads/release') {
            return $branchRef;
        }

        return exec('git pull');
    }
}
